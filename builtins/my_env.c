/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:14:59 by zatais            #+#    #+#             */
/*   Updated: 2025/06/19 18:14:59 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"

int	is_var_exist(t_env *env, char *var)
{
	while (env)
	{
		if (!ft_strncmp(env->value, var, sizeof(var)))
			return (1);
		env = env->next;
	}
	return (0);
}

int	my_env(t_env *env, char **args)
{
	if (*args)
		return (cmd_error("env", NULL, "No such file or directory"), 1);
	if (ft_strcmp(*args, NULL) && !is_var_exist(env, "PATH="))
		return (cmd_error("env", NULL, "No such file or directory"), 1);
	while (env)
	{
		if (ft_strchr(env->value, '='))
		{
			printf("%s\n", env->value);
			env = env->next;
		}
	}
	return (0);
}

/*int	main(int ac, char **av, char **envp)
{
	t_shell	*shell;
	char	**args;

	shell = malloc(sizeof(t_shell));
	copy_env(envp, &shell->env);
	args = malloc(24);
	args[0] = "unset";
	args[1] = "PATH";
  args[2] = NULL;
	//my_unset(&shell->env, args + 1);
	my_env(shell->env, args + 2);
	free_env(shell->env);
  free(shell);
  free(args);
}*/
