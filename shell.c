#include "exec.h"
#include <signal.h>
#include <sys/wait.h>
int		execute_builtin(t_shell *shell, t_command *cmd);
int		is_builtin(char *cmd);
char	*get_env_value(t_env *env, const char *key);
char	*find_executable(t_env *env, char *cmd);
char	**convert_env(t_env *env);
int		setup_redirections(t_redir *redirs);
void	restore_std_fds(int stdin_copy, int stdout_copy);
void	execute_pipeline(t_shell *shell);
void	handle_child_process(t_shell *shell, t_command *cmd, int in_fd,
			int out_fd);
void	wait_for_children(int num_commands, pid_t *pids, t_shell *shell);
void	execute(t_shell *shell);

int	execute_builtin(t_shell *shell, t_command *cmd)
{
	int	ret;

	ret = 127;
	if (ft_strncmp(cmd->args[0], "cd", 3) == 0)
		ret = my_cd(shell, &cmd->args[1]);
	else if (ft_strncmp(cmd->args[0], "echo", 5) == 0)
		ret = my_echo(&cmd->args[1]);
	else if (ft_strncmp(cmd->args[0], "env", 4) == 0)
		ret = my_env(shell->env, &cmd->args[1]);
	else if (ft_strncmp(cmd->args[0], "export", 7) == 0)
		ret = my_export(&shell->env, &cmd->args[1]);
	else if (ft_strncmp(cmd->args[0], "pwd", 4) == 0)
		ret = my_pwd(shell);
	else if (ft_strncmp(cmd->args[0], "unset", 6) == 0)
		ret = my_unset(&shell->env, &cmd->args[1]);
	else if (ft_strncmp(cmd->args[0], "exit", 5) == 0)
		ret = my_exit(&cmd->args[1], shell);
	return (ret);
}

int	is_builtin(char *cmd)
{
	if (ft_strcmp(cmd, "exit") == 0 || ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0 || ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "cd") == 0 || ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "echo") == 0)
		return (1);
	return (0);
}

void	execute(t_shell *shell)
{
	int			count;
	t_command	*tmp;
	int			saved_stdin;
	int			saved_stdout;

	if (!shell || !shell->cmd)
		return ;
	count = 0;
	tmp = shell->cmd;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	if (count == 1 && is_builtin(shell->cmd->args[0]))
	{
		saved_stdin = dup(STDIN_FILENO);
		saved_stdout = dup(STDOUT_FILENO);
		if (setup_redirections(shell->cmd->redirs) == -1)
		{
			shell->last_exit_status = 1;
			write(2, "minishell: redirection error\n", 29);
		}
		else
			shell->last_exit_status = execute_builtin(shell, shell->cmd);
		restore_std_fds(saved_stdin, saved_stdout);
		close(saved_stdin);
		close(saved_stdout);
	}
	else
		execute_pipeline(shell);
}

void	execute_pipeline(t_shell *shell)
{
	t_command	*cur;
	int			cmd_count;
	int			prev_pipe = -1;
	pid_t		*pids;
	t_command	*tmp_count;
	int			i;
	int			next_pipe[2];

	cur = shell->cmd;
	cmd_count = 0;
  cmd_counter(cmd);
	pids = malloc(cmd_count * sizeof(pid_t));
	if (!pids)
	{
		perror("minishell");
		return ;
	}
	i = 0;
	while (cur)
	{
		if (cur->next && pipe(next_pipe) == -1)
		{
			perror("minishell: pipe");
			break ;
		}
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("minishell: fork");
      if (cur->next)
      {
        close(next_pipe[0]);
        close(next_pipe[1]);
      }
			break ;
		}
		if (!pids[i])
		{
			if (i > 0)
			{
				dup2(prev_pipe, STDIN_FILENO);
				close(prev_pipe);
			}
			if (cur->next)
			{
				dup2(next_pipe[1], STDOUT_FILENO);
				close(next_pipe[0]);
				close(next_pipe[1]);
			}
			handle_child_process(shell, cur, -1, -1);
			exit(shell->last_exit_status);
		}
		else
		{
			if (i > 0)
				close(prev_pipe);
			if (cur->next)
      {
        prev_pipe = next_pipe[0];
        close(next_pipe[1]);
      }
		}
		cur = cur->next;
		i++;
	}
	if (prev_pipe != -1)
		close(prev_pipe);
	wait_for_children(i, pids, shell);
	free(pids);
}

void	handle_child_process(t_shell *shell, t_command *cmd, int in_fd,
		int out_fd)
{
	char	*full_path;
	char	**env_array;

	if (setup_redirections(cmd->redirs) == -1)
	{
		shell->last_exit_status = 1;
		exit(1);
	}
	if (in_fd != -1)
		dup2(in_fd, STDIN_FILENO);
	if (out_fd != -1)
		dup2(out_fd, STDOUT_FILENO);
	if (is_builtin(cmd->args[0]))
	{
		shell->last_exit_status = execute_builtin(shell, cmd);
		exit(shell->last_exit_status);
	}
	else
	{
		full_path = find_executable(shell->env, cmd->args[0]);
		if (!full_path)
		{
			write(2, "minishell: ", 11);
			write(2, cmd->args[0], ft_strlen(cmd->args[0]));
			write(2, ": command not found\n", 20);
			exit(127);
		}
		env_array = convert_env(shell->env);
		execve(full_path, cmd->args, env_array);
		perror("minishell");
		free(full_path);
		free(env_array);
		exit(126);
	}
}

int	setup_redirections(t_redir *redirs)
{
	t_redir		*cur;
	int			fd;
	const char	*filename;

	cur = redirs;
	while (cur)
	{
		if (cur->type == REDIR_HEREDOC)
			filename = cur->heredoc_file_name;
		else
			filename = cur->target;
		if (cur->type == REDIR_IN || cur->type == REDIR_HEREDOC)
		{
			fd = open(filename, O_RDONLY);
			if (fd == -1)
			{
				perror("minishell");
				return (-1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (cur->type == REDIR_OUT)
		{
			fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
			{
				perror("minishell");
				return (-1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (cur->type == REDIR_APPEND)
		{
			fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
			{
				perror("minishell");
				return (-1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		cur = cur->next;
	}
	return (0);
}

char	*find_executable(t_env *env, char *cmd)
{
	char	*path;
	char	*path_copy;
	char	*save;
	char	*dir;
	char	*full_path;
	int		found;
	char	*part1;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path = get_env_value(env, "PATH");
	if (!path)
		return (NULL);
	path_copy = ft_strdup(path);
	save = path_copy;
	dir = NULL;
	full_path = NULL;
	found = 0;
	while (*path_copy && !found)
	{
		dir = path_copy;
		while (*path_copy && *path_copy != ':')
			path_copy++;
		if (*path_copy == ':')
		{
			*path_copy = '\0';
			path_copy++;
		}
		if (*dir)
		{
			part1 = ft_strjoin(dir, "/");
			full_path = ft_strjoin(part1, cmd);
			free(part1);
			if (full_path && access(full_path, X_OK) == 0)
				found = 1;
			else if (full_path)
			{
				free(full_path);
				full_path = NULL;
			}
		}
	}
	free(save);
	return (full_path);
}

char	**convert_env(t_env *env)
{
	int		count;
	t_env	*tmp;
	char	**envp;
	int		i;

	count = 0;
	tmp = env;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	envp = malloc((count + 1) * sizeof(char *));
	if (!envp)
		return (NULL);
	tmp = env;
	i = 0;
	while (tmp)
	{
		envp[i] = tmp->value;
		i++;
		tmp = tmp->next;
	}
	envp[count] = NULL;
	return (envp);
}

/*char	*get_env_value(t_env *env, const char *key)
{
	size_t	len;
	t_env	*tmp;

	len = ft_strlen(key);
	tmp = env;
	while (tmp)
	{
		if (ft_strncmp(tmp->value, key, len) == 0 && tmp->value[len] == '=')
			return (tmp->value + len + 1);
		tmp = tmp->next;
	}
	return (NULL);
}
*/
void	restore_std_fds(int stdin_copy, int stdout_copy)
{
	dup2(stdin_copy, STDIN_FILENO);
	dup2(stdout_copy, STDOUT_FILENO);
}

void	wait_for_children(int num_commands, pid_t *pids, t_shell *shell)
{
	int		status;
	pid_t	wpid;
	int		last_status;
	int		i;

	last_status = 0;
	i = 0;
	while (i < num_commands)
	{
		wpid = waitpid(-1, &status, 0);
		if (wpid == -1)
		{
			perror("minishell: waitpid");
			i++;
			continue ;
		}
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
		i++;
	}
	shell->last_exit_status = last_status;
}

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;

	memset(&shell, 0, sizeof(t_shell));
	copy_env(envp, &shell.env);
	while (1337)
	{
		shell.r_line = readline("Minishell  > ");
		if (!shell.r_line)
			exit(0);
		shell.cmd = malloc(sizeof(t_command));
		shell.cmd->args = ft_split(shell.r_line, ' ');
		shell.cmd->redirs = NULL;
		shell.cmd->next = NULL;
		shell.cmd->prev = NULL;
		if (ft_strlen(shell.r_line))
		{
			add_history(shell.r_line);
			execute(&shell);
			printf("exit status : %d \n", shell.last_exit_status);
		}
		free(shell.r_line);
	//	free_command(shell.cmd);
	}
}
