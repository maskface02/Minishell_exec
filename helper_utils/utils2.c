#include "../exec.h"

t_env	*create_node(char *envp)
{
	t_env	*env;

	env = malloc(sizeof(t_env));
	if (!env)
		return (NULL);
	env->value = envp;
	env->next = NULL;
	return (env);
}

t_env	*last_node(t_env *node)
{
	while (node && node->next)
		node = node->next;
	return (node);
}

int	add_back(t_env **env, t_env *new)
{
	t_env	*last;

	if (!new)
		return (0);
	if (!(*env))
	{
		*env = new;
		return (1);
	}
	last = last_node(*env);
	last->next = new;
	return (1);
}

int	copy_env(char **envp, t_env **env)
{
	int		i;
	t_env	*new_node;

	i = -1;
	while (envp[++i])
	{
		new_node = create_node(envp[i]);
		if (!new_node)
			return (free_env(*env), 0);
		add_back(env, new_node);
	}
	return (1);
}
