/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   by: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   created: 2025/07/14 02:38:49 by zatais            #+#    #+#             */
/*   updated: 2025/07/14 04:06:50 by zatais           ###   ########.fr       */
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
	*fd = open(filename, o_rdonly);
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
	if (rd->type == redir_out)
	{
		*fd = open(filename, o_wronly | o_creat | o_trunc, 0644);
		if (*fd == -1)
		{
			perror("minishell");
			return (0);
		}
		dup2(*fd, 1);
		close(1);
	}
	else if (rd->type == redir_append)
	{
		*fd = open(filename, o_wronly | o_creat | o_append, 0644);
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
		if (rd->type == redir_heredoc)
			filename = rd->heredoc_file_name;
		if (rd->type == redir_in || rd->type == redir_heredoc)
		{
			if (!open_in(&fd, filename))
				return (0);
		}
		else 
      if (!open_out(&fd, filename, rd))
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

int	exec_pipeline(t_shell *shell)
{
	return (0);
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
			shell->last_exit_status = 1;
		else
			shell->last_exit_status = execute_builtin(shell);
		restore_stds(tmp_in, tmp_out);
		close(tmp_in);
		close(tmp_out);
	}
	else
		shell->last_exit_status = exec_pipeline(shell);
}
