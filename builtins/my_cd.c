#include "../exec.h"

int	count_arguments(char **args)
{
	int	count;

	count = 0;
	if (!args)
		return (0);
	while (args[count])
		count++;
	return (count);
}

char	*get_env_value(t_env *env, char *key)
{
	size_t	key_len;
	char	*val;

	key_len = ft_strlen(key);
	while (env)
	{
		if (!ft_strncmp(env->value, key, key_len) && (env->value[key_len] == '='
				|| env->value[key_len] == '\0'))
		{
			val = ft_strchr(env->value, '=');
			if (val)
				return (val + 1);
		}
		env = env->next;
	}
	return (NULL);
}

char	*get_path(t_shell *shell, char **args)
{
	int		count;
	char	*home;
	char	*oldpwd;

	count = count_arguments(args);
	if (count == 0 || !ft_strcmp(args[0], "~"))
	{
		home = get_env_value(shell->env, "HOME");
		if (!home)
			cmd_error("cd", NULL, "HOME not set");
		return (home);
	}
	if (!ft_strcmp(args[0], "-"))
	{
		oldpwd = get_env_value(shell->env, "OLDPWD");
		if (!oldpwd)
			cmd_error("cd", NULL, "OLDPWD not set");
		return (oldpwd);
	}
	return (args[0]);
}

int	update_pwd_vars(t_shell *shell, char *oldpwd, char *new)
{
	char	*oldpwd_arg;
	char	*pwd_arg;
	char	*export_args[3];
	int		exp_ret;

	oldpwd_arg = ft_strjoin("OLDPWD=", oldpwd, &shell->gc);
	pwd_arg = ft_strjoin("PWD=", new, &shell->gc);
	export_args[0] = oldpwd_arg;
	export_args[1] = pwd_arg;
	export_args[2] = NULL;
	exp_ret = my_export(&shell->env, export_args + 1, &shell->gc);
	return (!exp_ret);
}

int	update_cwd(t_shell *shell, char *new_path, t_gc_node **gc)
{
	char	*new_cwd;

	new_cwd = ft_strdup(new_path, &shell->gc);
  gc_remove(gc, shell->cwd);
	shell->cwd = new_cwd;
	return (1);
}

int	my_cd(t_shell *shell, char **args, t_gc_node **gc)
{
	char	*oldpwd;
	char	*path;
	int		print;
	char	*new;
	int		status;

	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
	{
		cmd_error("cd", "getcwd", strerror(errno));
		return (1);
	}
	gc_add(&shell->gc, oldpwd);
	if (count_arguments(args) > 1)
	{
		cmd_error("cd", NULL, "too many arguments");
		return (1);
	}
	path = get_path(shell, args);
	if (!path)
		return (1);
	print = (args[0] && !ft_strcmp(args[0], "-"));
	if (chdir(path))
	{
		cmd_error("cd", path, strerror(errno));
		return (1);
	}
	new = getcwd(NULL, 0);
	if (!new)
		new = ft_strdup(path, &shell->gc);
	else
		gc_add(&shell->gc, new);
	if (print)
		write(1, new, ft_strlen(new));
	if (print)
		write(1, "\n", 1);
	status = 0;
	if (!update_pwd_vars(shell, oldpwd, new))
		status = 1;
	if (!update_cwd(shell, new, gc))
		status = 1;
	return (status);
}
