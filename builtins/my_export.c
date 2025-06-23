/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 22:30:44 by zatais            #+#    #+#             */
/*   Updated: 2025/06/23 01:32:42 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../exec.h"

char	*extract_var(char *str)
{
	char	*eq;
	size_t	len;
	char	*name;

	eq = ft_strchr(str, '=');
	if (eq)
		len = eq - str;
	else
		len = ft_strlen(str);
	name = ft_strndup(str, len) // and here
	return (name);
}

int	compare_env_vars(char *s1, char *s2, int *error)
{
	char	*var1;
	char	*var2;
	int		result;

	var1 = extract_var(s1);
	if (!var1)
		return (*error = 1, 0);
	var2 = extract_var(s2);
	if (!var2)
		return (*error = 1, 0);
	result = ft_strcmp(var1, var2);// error
	free(var1);
	free(var2);
	return (result);
}

t_env	*find_min_node(t_env *current, int *error)
{
	t_env	*min;
	t_env	*comp;

	min = current;
	comp = current->next;
	while (comp)
	{
		if (compare_env_vars(min->value, comp->value, error) > 0)
			min = comp;
		if (*error)
			break ;
		comp = comp->next;
	}
	return (min);
}

int	sort_env(t_env *exp)
{
	t_env	*current;
	char	*swap;
	t_env	*min;
	int		error;

	current = exp;
	error = 0;
	while (current)
	{
		min = find_min_node(current, &error);
		if (error)
			return (0);
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

int	copy(t_env *env, t_env **exp)
{
	t_env	*new_node;
	char	*value;

	while (env)
	{
		value = ft_strdup(env->value);
		if (!value)
			return (0);
		new_node = create_node(value);
		if (!new_node)
			return (free(value), 0);
		add_back(exp, new_node);
		env = env->next;
	}
	return (1);
}

int	copy_sort(t_env *env, t_env **exp)
{
	t_env	*tmp;

	if (!copy(env, exp))
		return (0);
	if (*exp)
		if (!sort_env(*exp))
			return (0); // segfault here
	/*tmp = *exp;
	while (tmp)
	{
		printf("%s\n", tmp->value);
		tmp = tmp->next;
	}*/
	return (1);
}

int	valid_identifier(char *name)
{
	if (!ft_isalpha(*name) && *name != '_')
		return (0);
	name++;
	while (*name)
	{
		if (!ft_isalnum(*name) && *name != '_')
			return (0);
		name++;
	}
	return (1);
}

t_env	*find_env_var(t_env *env, char *name)
{
	while (env)
	{
		if (!ft_strncmp(env->value, name, ft_strlen(name))
			&& env->value[ft_strlen(name)] == '=')
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
			printf("declare -x %s\n", exp->value);
		exp = exp->next;
	}
}

int	create_new_node(t_env **env, char *arg)
{
	t_env	*new_node;

	new_node = create_node(arg);
	if (!new_node)
		return (0);
	add_back(env, new_node);
	return (1);
}

int	process_argument(t_env **env, char *arg)
{
	char	*eq;
	char	*name;
	int		ret;
	t_env	*node;

	eq = ft_strchr(arg, '=');
	ret = 0;
	if (eq)
	{
		name = ft_strndup(arg, eq - arg);
		if (!name)
			return (1);
	}
	if (!valid_identifier(name))
	{
		ft_putstr_fd("export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (free(name), 0);
	}
	else
	{
		node = find_env_var(*env, name);
		if (node)
			(1) && (free(node->value), node->value = arg, ret = 1);
		else
			ret = create_new_node(env, arg);
	}
	return (free(name), ret);
}

int	handle_export_args(t_env **env, char **args)
{
	int	ret;
	int	i;

	ret = 0;
	i = -1;
	while (args[++i])
		if (!process_argument(env, args[i]))
			ret = 1;
	return (ret);
}

int	my_export(t_env **env, char **args)
{
	t_env	*copy_env;

	copy_env = NULL;
	if (!args || !args[0])
	{
		if (!copy_sort(*env, &copy_env))
			return (1);
		return (print_sorted_env(copy_env), 0);
	}
	return (handle_export_args(env, args));
}

int	main(int ac, char **av, char **envp)
{
	t_shell	*shell;
	t_env	*tmp;

	shell = malloc(sizeof(t_shell));
	shell->env = NULL;
	shell->exp_env = NULL;
	copy_env(envp, &shell->env);
	my_export(&shell->env, av + 1);
	/*tmp = shell->env;
	while (tmp)
	{
		printf("%s\n", tmp->value);
		tmp = tmp->next;
	}*/
	free_env(shell->env);
	free_env(shell->exp_env);
	free(shell);
	return (0);
}
