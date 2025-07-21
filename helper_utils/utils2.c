#include "../exec.h"

t_env	*create_node(char *envp, t_gc_node **gc)
{
	t_env	*env;

	env = gc_malloc(gc, sizeof(t_env));
	env->value = ft_strdup(envp, gc);
	env->next = NULL;
	return (env);
}

t_env	*last_node(t_env *node)
{
	while (node && node->next)
		node = node->next;
	return (node);
}

void	add_back(t_env **env, t_env *new)
{
	t_env	*last;

  if (!(*env))
	{
		*env = new;
		return ;
	}
	last = last_node(*env);
	last->next = new;
	return ;
}

int	copy_env(char **envp, t_env **env, t_gc_node **gc)
{
	int		i;
	t_env	*new_node;

	i = -1;
	while (envp[++i])
	{
		new_node = create_node(envp[i], gc);
		add_back(env, new_node);
	}
	return (1);
}
