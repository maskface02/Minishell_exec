/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   by: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   created: 2025/07/14 02:38:49 by zatais            #+#    #+#             */
/*   Updated: 2025/07/20 06:41:03 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

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

char	**convert_env(t_shell *shell, t_gc_node **cpgc)
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
	envp = gc_malloc(cpgc, (count + 1) * sizeof(char *));
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

char *find_bin(char *arg, t_env *env, t_gc_node **cpgc)
{
  char *full_path;
  //check my_old pipex project before using interpreter methode
  return (full_path);
}

int is_not_found(t_shell *shell, t_command *cur_cmd, char **full_path, t_gc_node **cpgc)
{
  if (!*cur_cmd->args[0])
	{
    cmd_error(cur_cmd->args[0], NULL, "command not found");
		shell->last_exit_status = 127;
    return (1);
	}
  if (ft_strchr(cur_cmd->args[0], '/'))
    *full_path = ft_strdup(cur_cmd->args[0], cpgc);
  else
    *full_path = find_bin(cur_cmd->args[0], shell->env, cpgc);// TODO
  if (!*full_path)
  {
    cmd_error(cur_cmd->args[0], NULL, "command not found");
    shell->last_exit_status = 127;
    return (1);
  }
  if (access(*full_path, F_OK))
  {
    cmd_error(*full_path, NULL, "command not found\n");
    gc_clean(cpgc);
    shell->last_exit_status = 127;
    return (1);
  }
  return (0);
}

int is_directory(char *full_path)
{
  return (1);
}
int direc_perm(t_shell *shell, t_command *cur_cmd, char *full_path, t_gc_node **cpgc)
{

  if (is_directory(full_path))// 
  {
    cmd_error(full_path, NULL, "is a directory\n");
    gc_clean(cpgc);
    shell->last_exit_status  = 126;
    return (1);
  }
  if (access(full_path, X_OK))
  {
    cmd_error(full_path, NULL, "permission denied\n");// check the correct syntax
    gc_clean(cpgc);
    shell->last_exit_status = 126;
    return (1);
  }
  return (0);
}

void	exec_child(t_shell *shell, t_command *cur_cmd)
{
	char	*full_path;
	char	**env_array;
  t_gc_node *cpgc;

	if (redirect(cur_cmd->redirs) == -1)
	{
		shell->last_exit_status = 1;
		exit(1);
	}
  if (is_builtin(cur_cmd->args[0]))
	{
		shell->last_exit_status = execute_builtin(shell);
		exit(shell->last_exit_status);
	}
  if (is_not_found(shell, cur_cmd, &full_path, &cpgc))
    exit(shell->last_exit_status);
  if (direc_perm(shell,cur_cmd, full_path, &cpgc))
      exit(shell->last_exit_status);
  env_array = convert_env(shell, &cpgc);
  execve(full_path, cur_cmd->args, env_array);
  ft_putstr_fd("minishell: ", 2);
  ft_putstr_fd(strerror(errno), 2);
  ft_putstr_fd("\n", 2);
  gc_clean(&cpgc);
  exit(126);
}

int	exec_pipeline(t_shell *shell)
{
	t_command	*cmd;
	int			count;
	int			next_pipe[2];
	pid_t		*pids;
	int			i;
	int			prev_pipe;

	cmd = shell->cmd;
	count = cmd_counter(cmd);
	prev_pipe = -1;
  i = 0;
	pids = gc_malloc(&shell->gc, count * sizeof(pid_t));
	while (cmd)
	{
		if (cmd->next && pipe(next_pipe) == -1)
		{
			perror("minishell");
			break ;
		}
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("minishell: fork");
			if (cmd->next)
			{
				close(next_pipe[0]);
				close(next_pipe[1]);
			}
			break ;
		}
		if (!pids[i])
		{
			// signal_handler
			if (i > 0)
			{
				dup2(prev_pipe, 0);
				close(prev_pipe);
			}
			if (cmd->next)
			{
				dup2(next_pipe[1], 1);
				close(next_pipe[0]);
				close(next_pipe[1]);
			}
			exec_child(shell, cmd);
			exit(shell->last_exit_status);
		}
		else
		{
			if (i > 0)
				close(prev_pipe);
			if (cmd->next)
			{
				prev_pipe = next_pipe[0];
				close(next_pipe[1]);
			}
		}
		cmd = cmd->next;
		i++;
	}
	if (prev_pipe != -1)
		close(prev_pipe);
/* int j = 0, status;
    while (j < i)
    {
        waitpid(pids[j], &status, 0);
        j++;
    }
    if (i > 0 && WIFEXITED(status))
        shell->last_exit_status = WEXITSTATUS(status);*/
    gc_remove(&shell->gc, pids);
    return 0;
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
		shell->last_exit_status = exec_pipeline(shell);
}
