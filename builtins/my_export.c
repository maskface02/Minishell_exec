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

char    *extract_var(char *str)
{
        char    *eq;
        size_t  len;
        char    *name;

        eq = ft_strchr(str, '=');
        if (eq)
                len = eq - str;
        else
                len = ft_strlen(str);
        name = ft_strndup(str, len);
        return (name);
}

int     compare_env_vars(char *s1, char *s2)
{
        char    *var1;
        char    *var2;
        int             result;

        var1 = extract_var(s1);
        if (!var1)
                var2 = extract_var(s2);
        if (!var2)
                return (0);
        result = ft_strcmp(var1, var2);
        free(var1);
        free(var2);
        return (result);
}

t_env   *find_min_node(t_env *start)
{
        t_env   *min;
        t_env   *comp;

        min = start;
        comp = start->next;
        while (comp)
        {
                if (compare_env_vars(min->value, comp->value) > 0)
                        min = comp;
                comp = comp->next;
        }
        return (min);
}

void    sort_list(t_env *head)
{
        t_env   *current;
        char    *swap;
        t_env   *min;

        current = head;
        while (current)
        {
                min = find_min_node(current);
                if (min != current)
                {
                        swap = current->value;
                        current->value = min->value;
                        min->value = swap;
                }
                current = current->next;
        }
}

int     create_sorted_list(t_env *env, t_env **exp)
{
        t_env   *new_node;
        char    *value_dup;

        while (env)
        {
                value_dup = ft_strdup(env->value);
                if (!value_dup)
                        return (0);
                new_node = create_node(value_dup);
                if (!new_node)
                {
                        free(value_dup);
                        return (0);
                }
                add_back(exp, new_node);
                env = env->next;
        }
        return (1);
}

int     copy_sort(t_env *env, t_env **exp)
{
        if (*exp)
        {
                free_env(*exp);
                *exp = NULL;
        }
        if (!create_sorted_list(env, exp))
                return (0);
        if (*exp)
                sort_list(*exp);
        return (1);
}

int     valid_identifier(char *name)
{
        if (!name || !*name)
                return (0);
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

t_env   *find_env_node(t_env *env, char *name)
{
        size_t  len;

        len = ft_strlen(name);
        while (env)
        {
                if (ft_strncmp(env->value, name, len) == 0)
                {
                        if (env->value[len] == '=')
                                return (env);
                }
                env = env->next;
        }
        return (NULL);
}

void    print_env_var(char *value)
{
        char    *eq;

        eq = ft_strchr(value, '=');
        if (eq)
        {
                *eq = '\0';
                printf("declare -x %s=\"%s\"\n", value, eq + 1);
                *eq = '=';
        }
        else
                printf("declare -x %s\n", value);
}

void    print_sorted_env(t_env *exp)
{
        while (exp)
        {
                print_env_var(exp->value);
                exp = exp->next;
        }
}

int     update_existing(t_env *node, char *arg)
{
        char    *new_value;

        new_value = ft_strdup(arg);
        if (!new_value)
                return (1);
        free(node->value);
        node->value = new_value;
        return (0);
}

int     create_new_with_equal(char *arg)
{
        char    *new_val;
        t_env   *new_node;

        new_val = ft_strdup(arg);
        if (!new_val)
                return (1);
        new_node = create_node(new_val);
        if (!new_node)
        {
                free(new_val);
                return (1);
        }
        return (0);
}

int     create_new_without_equal(char *name)
{
        char    *new_val;
        t_env   *new_node;

        new_val = ft_strjoin(name, "=");
        if (!new_val)
                return (1);
        new_node = create_node(new_val);
        if (!new_node)
        {
                free(new_val);
                return (1);
        }
        return (0);
}

int     create_new_node(t_env **env, char *name, char *arg, char *eq)
{
        t_env   *new_node;
        char    *new_val;

        if (eq)
                new_val = ft_strdup(arg);
        else
                new_val = ft_strjoin(name, "=");
        if (!new_val)
                return (1);
        new_node = create_node(new_val);
        if (!new_node)
        {
                free(new_val);
                return (1);
        }
        add_back(env, new_node);
        return (0);
}

int     process_argument(t_env **env, char *arg)
{
        char    *eq;
        char    *name;
        int             ret;
        t_env   *node;

        eq = ft_strchr(arg, '=');
        ret = 0;
        if (eq)
                name = ft_strndup(arg, eq - arg);
        else
                name = ft_strdup(arg);
        if (!name)
                return (1);
        if (!valid_identifier(name))
        {
                ft_putstr_fd("export: `", 2);
                ft_putstr_fd(arg, 2);
                ft_putstr_fd("': not a valid identifier\n", 2);
                ret = 1;
        }
        else
        {
                node = find_env_node(*env, name);
                if (node && eq)
                        ret = update_existing(node, arg);
                else if (!node)
                        ret = create_new_node(env, name, arg, eq);
        }
        free(name);
        return (ret);
}

int     handle_export_args(t_env **env, char **args)
{
        int     ret;
        int     i;

        ret = 0;
        i = 0;
        while (args[i])
        {
                if (process_argument(env, args[i]))
                        ret = 1;
                i++;
        }
        return (ret);
}

int     my_export(t_env **env, t_env **exp, char **args)
{
        if (!args || !args[0])
        {
                if (!copy_sort(*env, exp))
                        return (1);
                print_sorted_env(*exp);
                return (0);
        }
        return (handle_export_args(env, args));
}

int     main(int ac, char **av, char **envp)
{
        t_shell *shell;
        t_env   *tmp;

        shell = malloc(sizeof(t_shell));
        shell->env = NULL;
        shell->exp_env = NULL;
        copy_env(envp, &shell->env);
        my_export(&shell->env, &shell->exp_env, av + 1);
        tmp = shell->env;
        while (tmp)
        {
                printf("%s\n", tmp->value);
                tmp = tmp->next;
        }
        free_env(shell->env);
        free_env(shell->exp_env);
        free(shell);
        return (0);
}
