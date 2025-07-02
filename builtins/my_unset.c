/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:15:10 by zatais            #+#    #+#             */
/*   Updated: 2025/06/19 18:15:10 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"

void	free_and_update(t_env **prev, t_env **current, t_env **env)
{
	t_env	*temp;

	temp = *current;
	if (!*prev)
	{
		*env = temp->next;
		*current = *env;
	}
	else
	{
		(*prev)->next = temp->next;
		*current = (*prev)->next;
	}
	free(temp->value);
	free(temp);
}
// still need a check for unset invalid arg like 45hello or !hello
int	my_unset(t_env **env, char **args)
{
	int		i;
	t_env	*current;
	t_env	*prev;

	if (!args || !*args)
		return (0);
	i = -1;
	while (args[++i])
	{
		current = *env;
    prev = NULL;
		while (current)
		{
			if (!ft_strncmp(current->value, args[i], ft_strlen(args[i]))
				&& (current->value[ft_strlen(args[i])] == '='
				|| current->value[ft_strlen(args[i])] == '\0'))
				free_and_update(&prev, &current, env);
			else
				(1) && (prev = current, current = current->next);
		}
	}
	return (0);
}

/*int	main(int ac, char **av, char **envp)
{
	t_shell	*shell;
	t_env	*tmp;

	shell = malloc(sizeof(t_shell));
	if (!copy_env(envp, &shell->env))
		return (free(shell), 1);
	my_unset(&shell->env, av + 1);
	tmp = shell->env;
	while (tmp)
	{
		printf("%s\n", tmp->value);
		tmp = tmp->next;
	}
	free_env(shell->env);
	free(shell);
	return (0);
}*/
