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
#include "../exec.h"

int	compare_env_vars(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (1)
	{
		if (s1[i] == '=' || s1[i] == '\0')
		{
			if (s2[i] == '=' || s2[i] == '\0')
				return (0);
			else
				return (-1);
		}
		if (s2[i] == '=' || s2[i] == '\0')
			return (1);
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
}
int	copy_sort(t_env *env, t_env **exp)
{
	t_env	*tmp;
	t_env	*new_node;
	t_env	*current;
	t_env	*min;
	t_env	*comp;
	char	*swap;

	tmp = env;
	*exp = NULL;
	while (env)
	{
		new_node = create_node(env->value);
		if (!new_node)
		{
			free_env(*exp);
			return (0);
		}
		add_back(exp, new_node);
		env = env->next;
	}
	env = tmp;
	if (*exp == NULL)
		return (1);
	current = *exp;
	while (current)
	{
		min = current;
		comp = current->next;
		while (comp)
		{
			if (compare_env_vars(min->value, comp->value) > 0)
				min = comp;
			comp = comp->next;
		}
		if (min != current)
		{
			swap = current->value;
			current->value = min->value;
			min->value = swap;
		}
		current = current->next;
	}
	return (1);
}

int	valid_identifier(char *name)
{
	if (!name || !*name)
		return (0);
	if (!(ft_isalpha(*name) && *name != '_'))
		return (0);
	name++;
	while (*name)
	{
		if (!(ft_isalnum(*name) && *name != '_'))
			return (0);
		name++;
	}
	return (1);
}

t_env	*find_env_node(t_env *env, char *name)
{
	size_t	len;

	len = ft_strlen(name);
	while (env)
	{
		if (ft_strncmp(env->value, name, len) == 0 && env->value[len] == '=')
			return (env);
		env = env->next;
	}
	return (NULL);
}

void	print_sorted_env(t_env *exp)
{
	char	*eq;

	while (exp)
	{
		eq = ft_strchr(exp->value, '=');
		if (eq)
		{
			*eq = '\0';
			printf("declare -x %s=\"%s\"\n", exp->value, eq + 1);
			*eq = '=';
		}
		else
		{
			printf("declare -x %s\n", exp->value);
		}
		exp = exp->next;
	}
}

int	process_argument(t_env **env, char *arg)
{
	char	*eq;
	char	*tmp;
	char	*name;
	int		name_valid;
	t_env	*node;
	t_env	*new_node;
	char	*new_val;

	eq = ft_strchr(arg, '=');
	if (eq)
		name = ft_strndup(arg, eq - arg);
	else
		name = ft_strdup(arg);
	name_valid = valid_identifier(name);
	if (!name_valid)
	{
		ft_putstr_fd("export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		free(name);
		return (1);
	}
	node = find_env_node(*env, name);
	if (node)
	{
		if (eq)
		{
			free(node->value)// here the error ;
			node->value = ft_strdup(arg);
		}
	}
	else
	{
		if (eq)
		{
			tmp = ft_strdup(arg);
			new_node = create_node(tmp);
			free(tmp);
		}
		else
		{
			new_val = ft_strjoin(name, "=");
			new_node = create_node(new_val);
		}
		if (!new_node)
		{
			free(name);
			return (1);
		}
		add_back(env, new_node);
	}
	free(name);
	return (0);
}

int	my_export(t_env **env, t_env **exp, char **args)
{
	int	ret;
	int	i;

	ret = 0;
	if (!args || !args[0])
	{
		if (!copy_sort(*env, exp))
			return (1);
		print_sorted_env(*exp);
	}
	else
	{
		i = 0;
		while (args[i])
		{
			if (process_argument(env, args[i]))
				ret = 1;
			i++;
		}
		if (!copy_sort(*env, exp))
			return (1);
	}
	return (ret);
}

int	main(int ac, char **av, char **envp)
{
	t_shell	*shell;
	t_env	*tmp;

	shell = malloc(sizeof(t_shell));
  shell->env = NULL;
	copy_env(envp, &shell->env);
	my_export(&shell->env, &shell->exp_env, av + 1);
	tmp = shell->env;
	while (tmp)
	{
		printf("%s \n", tmp->value);
		tmp = tmp->next;
	}
	free_env(shell->env);
	free_env(shell->exp_env);
	free(shell);
}
