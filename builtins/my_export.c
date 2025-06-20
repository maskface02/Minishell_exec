/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 22:30:44 by zatais            #+#    #+#             */
/*   Updated: 2025/06/19 22:30:44 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "exec.h"

int copy_sort(t_env *env, t_env **exp)
{
	int		i;
	t_env	*new_node;
  t_env *tmp;
	i = -1;
  tmp = env;
	while (env)
	{
		new_node = create_node(env->value);
		if (!new_node)
			return (free_env(*exp), 0);
		add_back(exp, new_node);
    env = env->next;
	}
  env = tmp;
  while (env)
  {
    if (ft_strcmp(exp->value, exp->next->value) > 0)

      

  }
	return (1);
}

int my_export(t_env **env, t_env **exp, char **args)
{
  int i;

  i = -1;
  if (!copy_sort(env, &exp))
    return (1);
  while (args[++i])
  {

  }
  return (0);
}

int main(int ac, char **av, char ** envp)
{
  t_shell *shell;

  shell = malloc(sizeof(t_shell));
  copy_env(envp, shell->env);
  my_export(&shell->env, &shell->exp_env);
}
