#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROMPT "minishell"
#define REDIR_IN 1
#define REDIR_OUT 2
#define REDIR_APPEND 3
#define REDIR_HEREDOC 4

typedef struct s_redir
{
	int					type;
	char				*target;
	int					fd;
	struct s_redir		*next;
}						t_redir;

typedef struct s_command
{
	char				**args;
	t_redir				*redirs;
	struct s_command	*next;
	struct s_command	*prev;
}						t_command;

typedef struct s_env
{
	char				*value;
	struct s_env		*next;
}						t_env;

typedef struct s_shell
{
	char				*r_line;
	t_command			*cmd;
	t_env				*env;
	int					last_exit_status;
	char				*cwd;
}						t_shell;

int						is_builtin(char *cmd);
int						execute_builtin(t_shell *shell, char **args);
void					execute_external(t_command *cmd, t_env *env);
char					*find_in_path(char *cmd, t_env *env);
char					**env_to_array(t_env *env);
int						apply_redirections(t_command *cmd);
int						preprocess_heredocs(t_shell *shell);
int						count_commands(t_shell *shell);
int						is_single_builtin(t_shell *shell);
void					execute_single_builtin(t_shell *shell);
void					execute_command_pipeline(t_shell *shell);
void					cleanup_heredocs(t_shell *shell);

void	start_exec(t_shell *shell)
{
	if (!shell->cmd)
		return ;
	if (!preprocess_heredocs(shell))
	{
		shell->last_exit_status = 1;
		return ;
	}
	if (is_single_builtin(shell))
		execute_single_builtin(shell);
	else
		execute_command_pipeline(shell);
	cleanup_heredocs(shell);
}

int	preprocess_heredocs(t_shell *shell)
{
	t_command	*cmd;
	t_redir		*redir;
	int			pipe_fd[2];
	char		*line;

	cmd = shell->cmd;
	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
			{
				if (pipe(pipe_fd) == -1)
				{
					perror("minishell: pipe");
					return (0);
				}
				while (1)
				{
					line = readline("> ");
					if (!line)
					{
						write(1, "\n", 1);
						break ;
					}
					if (strcmp(line, redir->target) == 0)
					{
						free(line);
						break ;
					}
					write(pipe_fd[1], line, strlen(line));
					write(pipe_fd[1], "\n", 1);
					free(line);
				}
				close(pipe_fd[1]);
				redir->fd = pipe_fd[0];
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (1);
}

int	is_single_builtin(t_shell *shell)
{
	if (!shell->cmd)
		return (0);
	if (shell->cmd->next)
		return (0); // Pipeline exists
	return (is_builtin(shell->cmd->args[0]));
}

void	execute_single_builtin(t_shell *shell)
{
	t_command	*cmd;
	int			saved_stdin;
	int			saved_stdout;
	int			status;

	cmd = shell->cmd;
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	status = 0;
	if (apply_redirections(cmd) == 0)
		status = execute_builtin(shell, cmd->args);
	else
		status = 1;
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	shell->last_exit_status = status;
}

void	execute_command_pipeline(t_shell *shell)
{
	t_command	*cmd;
	int			num_cmds;
	pid_t		*pids;
	int			prev_pipe_read;
	int			pipe_fd[2];
	int			i;
	int			ret;
		int status;

	cmd = shell->cmd;
	num_cmds = count_commands(shell);
	pids = malloc(num_cmds * sizeof(pid_t));
	prev_pipe_read = -1;
	i = 0;
	if (!pids)
	{
		perror("minishell: malloc");
		return ;
	}
	while (cmd)
	{
		if (i < num_cmds - 1 && pipe(pipe_fd) < 0)
		{
			perror("minishell: pipe");
			break ;
		}
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("minishell: fork");
			break ;
		}
		if (pids[i] == 0)
		{ // Child process
			// Input from previous command
			if (i > 0)
			{
				dup2(prev_pipe_read, STDIN_FILENO);
				close(prev_pipe_read);
			}
			// Output to next command
			if (i < num_cmds - 1)
			{
				close(pipe_fd[0]);
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[1]);
			}
			// Apply file redirections
			if (apply_redirections(cmd) != 0)
				exit(1);
			// Execute command
			if (is_builtin(cmd->args[0]))
			{
				ret = execute_builtin(shell, cmd->args);
				exit(ret);
			}
			else
			{
				execute_external(cmd, shell->env);
				exit(127); // Shouldn't reach here
			}
		}
		else
		{ // Parent process
			// Close previous pipe read end
			if (i > 0)
				close(prev_pipe_read);
			// Set up next pipe
			if (i < num_cmds - 1)
			{
				close(pipe_fd[1]);
				prev_pipe_read = pipe_fd[0];
			}
			// Move to next command
			cmd = cmd->next;
			i++;
		}
	}
	// Close last pipe
	if (prev_pipe_read != -1)
		close(prev_pipe_read);
	// Wait for all children
	for (int j = 0; j < i; j++)
	{
		waitpid(pids[j], &status, 0);
		if (j == i - 1)
		{ // Last command
			if (WIFEXITED(status))
				shell->last_exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				shell->last_exit_status = 128 + WTERMSIG(status);
		}
	}
	free(pids);
}

int	apply_redirections(t_command *cmd)
{
	t_redir	*redir;
	int		fd;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == REDIR_IN)
		{
			fd = open(redir->target, O_RDONLY);
			if (fd < 0)
			{
				perror("minishell");
				return (1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == REDIR_HEREDOC)
		{
			dup2(redir->fd, STDIN_FILENO);
			close(redir->fd);
		}
		else if (redir->type == REDIR_OUT)
		{
			fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				perror("minishell");
				return (1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == REDIR_APPEND)
		{
			fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd < 0)
			{
				perror("minishell");
				return (1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		redir = redir->next;
	}
	return (0);
}

void	execute_external(t_command *cmd, t_env *env)
{
	char	*path;
	char	**env_arr;

	path = find_in_path(cmd->args[0], env);
	env_arr = env_to_array(env);
	if (!path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", cmd->args[0]);
		exit(127);
	}
	execve(path, cmd->args, env_arr);
	perror("minishell");
	free(path);
	// Free env_arr if needed
	exit(126);
}

int	count_commands(t_shell *shell)
{
	t_command	*cmd;
	int			count;

	cmd = shell->cmd;
	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

void	cleanup_heredocs(t_shell *shell)
{
	t_command	*cmd;
	t_redir		*redir;

	cmd = shell->cmd;
	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->fd != -1)
			{
				close(redir->fd);
				redir->fd = -1;
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}

// Example main function
int	main(int ac, char **av, char **env)
{
	t_shell	shell;
	char	*prompt;

	prompt = "minishell> ";
	(void)ac;
	(void)av;
	memset(&shell, 0, sizeof(t_shell));
	// Initialize environment (simplified)
	// shell.env = init_env(env);
	while (1)
	{
		shell.r_line = readline(prompt);
		if (!shell.r_line)
			exit(0);
		if (strlen(shell.r_line) > 0)
		{
			add_history(shell.r_line);
			// Parse input into commands and redirections
			// shell.cmd = parse_input(shell.r_line, shell.env);
			start_exec(&shell);
		}
		free(shell.r_line);
		shell.r_line = NULL;
		// Free commands and redirections
		// cleanup_commands(shell.cmd);
		shell.cmd = NULL;
	}
	return (0);
}
