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

int	compare_env_vars(char *s1, char *s2) // this function need norm
{
	char *eq1;
	char *eq2;
	int len2;
	int len1;
	int i;
	int min_len;

	eq1 = ft_strchr(s1, '=');
	if (eq1)
		len1 = eq1 - s1;
	else
		len1 = ft_strlen(s1);
	eq2 = ft_strchr(s2, '=');
	if (eq2)
		len2 = eq2 - s2;
	else
		len2 = ft_strlen(s2);
	min_len = len1;
	if (len2 < min_len)
		min_len = len2;
	i = 0;
	while (i <= min_len)
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

t_env	*find_min_node(t_env *current)
{
	t_env	*min;
	t_env	*comp;
	int		i;

	min = current;
	comp = current->next;
	i = 0;
	while (comp)
	{
		++i;
		if (compare_env_vars(min->value, comp->value) > 0)
			min = comp;
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
	int		i;

	i = 0;
	current = exp;
	error = 0;
	while (current)
	{
		++i;
		min = find_min_node(current);
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

int	copy_sort(t_env *env, t_env **exp)
{
	t_env	*new_node;
	int		i;

	i = 0;
	while (env)
	{
		++i;
		new_node = create_node(env->value);
		if (!new_node)
			return (0);
		add_back(exp, new_node);
		env = env->next;
	}
	if (!sort_env(*exp))
		return (0);
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
		if (exp->value[0] == '_' && (exp->value[1] == '\0'
				|| exp->value[1] == '='))
		{
			exp = exp->next;
			continue ;
		}
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

int	check_argument(char *arg, char **name, char **value, int *is_allocated)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	*is_allocated = 0;
	if (eq)
	{
		*name = ft_strndup(arg, eq - arg);
		if (!*name)
			return (0);
		*is_allocated = 1;
		*value = eq + 1;
	}
	else
	{
		*name = arg;
		*value = NULL;
	}
	return (1);
}

void	cmd_error2(char *arg, char *name, int is_allocated)
{
	ft_putstr_fd("Minishell: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	if (is_allocated)
		free(name);
}

int	update_env_var(t_env *node, char *new_value)
{
	char	*new;

	new = ft_strdup(new_value);
	if (!new)
		return (0);
	free(node->value);
	node->value = new;
	return (1);
}

int	process_argument(t_env **env, char *arg)
{
	char	*name;
	char	*value;
	int		is_allocated;
	int		ret;
	t_env	*node;

	ret = 1;
	if (!check_argument(arg, &name, &value, &is_allocated))
		return (1);
	if (!valid_identifier(name))
		return (cmd_error2(arg, name, is_allocated), 0);
	node = find_env_var(*env, name);
	if (node && ft_strchr(arg, '='))
		ret = update_env_var(node, arg);
	else if (!node)
		ret = create_new_node(env, arg);
	if (is_allocated)
		free(name);
	return (ret);
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
		return (print_sorted_env(copy_env), free_env(copy_env), 0);
	}
	return (handle_export_args(env, args));
}
// hundel same args like export hello hello only export one
// a segfault  in case export existing variable
/*int	main(int ac, char **av, char **envp)
{
	t_shell	*shell;
	t_env	*tmp;
	char	*arg;

	shell = malloc(sizeof(t_shell));
	shell->env = NULL;
	shell->exp_env = NULL;
	copy_env(envp, &shell->env);
	my_export(&shell->env, av + 1);
	arg = NULL;
	my_export(&shell->env, &arg);
	free_env(shell->env);
	free(shell);
	return (0);
}*/
