/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   by: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   created: 2025/07/14 02:38:49 by zatais            #+#    #+#             */
/*   Updated: 2025/07/20 20:58:42 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"

int	execute_builtin(t_shell *shell)
{
	int	ret;

	ret = 127;
	if (!ft_strncmp(shell->cmd->args[0], "cd", 3))
		ret = my_cd(shell, &shell->cmd->args[1], &shell->gc);
	else if (!ft_strncmp(shell->cmd->args[0], "echo", 5))
		ret = my_echo(&shell->cmd->args[1]);
	else if (!ft_strncmp(shell->cmd->args[0], "env", 4))
		ret = my_env(shell->env, &shell->cmd->args[1]);
	else if (!ft_strncmp(shell->cmd->args[0], "export", 7))
		ret = my_export(&shell->env, &shell->cmd->args[1], &shell->gc);
	else if (!ft_strncmp(shell->cmd->args[0], "pwd", 4))
		ret = my_pwd(shell);
	else if (!ft_strncmp(shell->cmd->args[0], "unset", 6))
		ret = my_unset(&shell->env, &shell->cmd->args[1], &shell->gc);
	else if (!ft_strncmp(shell->cmd->args[0], "exit", 5))
		ret = my_exit(&shell->cmd->args[1], shell);
	return (ret);
}

int	is_builtin(char *cmd)
{
	if (!ft_strcmp(cmd, "exit") || !ft_strcmp(cmd, "unset") || !ft_strcmp(cmd,
			"env") || !ft_strcmp(cmd, "export") || !ft_strcmp(cmd, "cd")
		|| !ft_strcmp(cmd, "pwd") || !ft_strcmp(cmd, "echo"))
		return (1);
	return (0);
}

int	cmd_counter(t_command *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		++count;
		cmd = cmd->next;
	}
	return (count);
}

int	open_in(int *fd, char *filename)
{
	*fd = open(filename, O_RDONLY);
	if (*fd == -1)
	{
		perror("minishell");
		return (0);
	}
	dup2(*fd, 0);
	close(*fd);
	return (1);
}

int	open_out(int *fd, char *filename, t_redir *rd)
{
	if (rd->type == REDIR_OUT)
	{
		*fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (*fd == -1)
		{
			perror("minishell");
			return (0);
		}
		dup2(*fd, 1);
		close(1);
	}
	else if (rd->type == REDIR_APPEND)
	{
		*fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (*fd == -1)
		{
			perror("minishell");
			return (0);
		}
		dup2(*fd, 1);
		close(*fd);
	}
	return (1);
}

int	redirect(t_redir *rd)
{
	int		fd;
	char	*filename;

	while (rd)
	{
		filename = rd->target;
		if (rd->type == REDIR_HEREDOC)
			filename = rd->heredoc_file_name;
		if (rd->type == REDIR_IN || rd->type == REDIR_HEREDOC)
		{
			if (!open_in(&fd, filename))
				return (0);
		}
		else if (!open_out(&fd, filename, rd))
			return (0);
		rd = rd->next;
	}
	return (1);
}

void	restore_stds(int stdin, int stdout)
{
	dup2(stdin, 1);
	dup2(stdout, 0);
}

char	**convert_env(t_shell *shell, t_gc_node **gc)
{
	int		count;
	t_env	*tmp;
	char	**envp;
	int		i;

	count = 0;
	tmp = shell->env;
	while (tmp)
	{
		++count;
		tmp = tmp->next;
	}
	envp = gc_malloc(gc, (count + 1) * sizeof(char *));
	if (!envp)
		return (NULL);
	tmp = shell->env;
	i = -1;
	while (tmp)
	{
		envp[++i] = tmp->value;
		tmp = tmp->next;
	}
	envp[count] = NULL;
	return (envp);
}

char	*find_bin(char *arg, t_env *env, t_gc_node **gc, int *f_err)
{
	char	*full_path;
	char	**paths;
	char	*path;
	int		i;

	path = get_env_value(env, "PATH");
	if (!path)
		return (NULL);
	if (!*path)
	{
		*f_err = 1; // empty path like PATH=;
		return (NULL);
	}
	paths = ft_split(path, ':', gc);
	i = -1;
	while (paths[++i])
	{
		full_path = ft_strjoin(ft_strjoin(paths[i], "/", gc), arg, gc);
		// check if binary is a directory
		if (!access(full_path, F_OK))
			return (full_path);
	}
	return (NULL);
}

int	is_not_found(t_shell *shell, t_command *cur_cmd, char **full_path,
		t_gc_node **gc)
{
	int	err;

	err = 0;
	if (!*cur_cmd->args[0])
	{
		cmd_error(cur_cmd->args[0], NULL, "command not found");
		gc_clean(gc);
		return (1);
	}
	if (ft_strchr(cur_cmd->args[0], '/'))
		*full_path = ft_strdup(cur_cmd->args[0], gc);
	else
		*full_path = find_bin(cur_cmd->args[0], shell->env, gc, &err);
	if (!*full_path || access(*full_path, F_OK))
	{
		if (!err)
			cmd_error(cur_cmd->args[0], NULL, "command not found");
		else
			cmd_error(cur_cmd->args[0], NULL, "No such file or directory");
		gc_clean(gc);
		return (1);
	}
	return (0);
}

int	is_dir(char *full_path)
{
	struct stat	statbuf;

	if (!stat(full_path, &statbuf))
		return (S_ISDIR(statbuf.st_mode));
	return (0);
}

int	dir_perm(char *full_path, t_gc_node **gc)
{
	if (is_dir(full_path))
	{
		cmd_error(full_path, NULL, "is a directory\n");
		gc_clean(gc);
		return (1);
	}
	if (access(full_path, X_OK))
	{
		cmd_error(full_path, NULL, "permission denied\n");
		gc_clean(gc);
		return (1);
	}
	return (0);
}

void	exec_child(t_shell *shell, t_command *cur_cmd)
{
	char	*full_path;
	char	**env_array;

	if (redirect(cur_cmd->redirs) == -1)
	{
		gc_clean(&shell->gc);
		exit(1);
	}
	if (is_builtin(cur_cmd->args[0]))
	{
		shell->last_exit_status = execute_builtin(shell);
		gc_clean(&shell->gc);
		exit(shell->last_exit_status);
	}
	if (is_not_found(shell, cur_cmd, &full_path, &shell->gc))
		exit(127);
	if (dir_perm(full_path, &shell->gc) == 1)
		exit(126);
	env_array = convert_env(shell, &shell->gc);
	execve(full_path, cur_cmd->args, env_array);
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(strerror(errno), 2);
	ft_putstr_fd("\n", 2);
	gc_clean(&shell->gc);
}

void	c_proc(t_shell *shell, int *next_pipe, int prev_pipe, int i)
{
	// signal_handler
	if (i > 0)
	{
		dup2(prev_pipe, 0);
		close(prev_pipe);
	}
	if (shell->cmd->next)
	{
		dup2(next_pipe[1], 1);
		close(next_pipe[0]);
		close(next_pipe[1]);
	}
	exec_child(shell, shell->cmd);
}

void	set_close(t_command *cmd, int *next_pipe, int *prev_pipe, int i)
{
	if (i > 0)
		close(*prev_pipe);
	if (cmd->next)
	{
		*prev_pipe = next_pipe[0];
		close(next_pipe[1]);
	}
}

void	wait_all(t_shell *shell, int *pids, int i, int count)
{
	int	j;
	int	status;

	j = 0;
	if (count > 1)
	{
		while (j < i)
		{
			waitpid(pids[j], &status, 0);
			j++;
		}
		if (i > 0 && WIFEXITED(status))
			shell->last_exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			shell->last_exit_status = 128 + WTERMSIG(status);
	}
	else
	{
		wait(&status);
		if (WIFEXITED(status))
			shell->last_exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			shell->last_exit_status = 128 + WTERMSIG(status);
	}
}

void	exec_pipeline(t_shell *shell, int count)
{
	int		next_pipe[2];
	pid_t	*pids;
	int		i;
	int		prev_pipe;

	prev_pipe = -1;
	i = 0;
	pids = gc_malloc(&shell->gc, count * sizeof(pid_t));
	while (shell->cmd)
	{
		if (shell->cmd->next && pipe(next_pipe) == -1)
		{
			perror("minishell");
			break ;
		}
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("minishell: fork");
			if (shell->cmd->next)
			{
				close(next_pipe[0]);
				close(next_pipe[1]);
			}
			break ;
		}
		if (!pids[i])
			c_proc(shell, next_pipe, prev_pipe, i);
		else
			set_close(shell->cmd, next_pipe, &prev_pipe, i);
		shell->cmd = shell->cmd->next;
		i++;
	}
	wait_all(shell, pids, --i, count);
	if (prev_pipe != -1)
		close(prev_pipe);
}

void	start_exec(t_shell *shell)
{
	int	count;
	int	tmp_in;
	int	tmp_out;

	count = cmd_counter(shell->cmd);
	if (count == 1 && is_builtin(shell->cmd->args[0]))
	{
		tmp_in = dup(0);
		tmp_out = dup(1);
		if (!redirect(shell->cmd->redirs))
		{
			shell->last_exit_status = 1;
			write(2, "minishell: redirection error\n", 29);
		}
		else
			shell->last_exit_status = execute_builtin(shell);
		restore_stds(tmp_in, tmp_out);
		close(tmp_in);
		close(tmp_out);
	}
	else
		exec_pipeline(shell, count);
}
