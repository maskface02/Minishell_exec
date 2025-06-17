/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 18:49:27 by zatais            #+#    #+#             */
/*   Updated: 2025/06/13 21:11:27 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"

int	is_alone(t_command *cmd)
{
	if (cmd->next)
		return (0);
	return (1);
}

int	exit_status(char *status, int *exit)
{
	int		ret;
	long	converted;
	int		overflow;
	int		i;

	overflow = 0;
	i = -1;
	while (is_space(status[++i]))
		;
	if (status[i] == '-')
		++i;
	while (status[i])
	{
		if (!(status[i] >= '0' && status[i] <= '9'))
			return (1);
		i++;
	}
	converted = ft_atol(status, &overflow);
	*exit = converted % 256;
	return (overflow);
}

void	exit_all(int status, t_shell *shell)
{
  // free allocated resources in shell struct
	free_env(shell->env);
  free_cmd(shell->cmd);
	free(shell);
	exit(status);
}

int	my_exit(char **args, t_shell *shell)
{
	int	n;
	int	status;
	int	error;

	if (is_alone(shell->cmd))
		ft_putendl_fd("exit", 2);
	n = args_counter(args);
	if (!n)
		return (0);
	error = exit_status(args[0], &status);
	if (error)
		return (cmd_error("exit", args[0], "numeric argument required"), 2);
	if (n > 1)
		return (cmd_error("exit", NULL, "too many arguments"), 1);
	if (is_alone(shell->cmd))
		exit_all(status, shell);
	return (status);
}

/*int	main(int ac, char **av)
{
	t_shell	*shell;
  t_command *cmd2;
	shell = malloc(sizeof(t_shell));
	shell->cmd = malloc(sizeof(t_command));
	shell->cmd->next = cmd2;
  int exit_code = my_exit(av + 1, shell);
  free(shell->cmd);
  free(shell);
	exit(exit_code);
}*/
