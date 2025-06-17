#include "../exec.h"

int	path_var_exist(t_env *env)
{
	while (env)
	{
		if (!ft_strncmp(env->value, "PATH=", 5))
			return (1);
		env = env->next;
	}
	return (0);
}

int	my_env(t_env *env, char **args)
{
	t_env	*tmp;

  if (*args)
    return (cmd_error("env", NULL, "too many arguments"), 1);
	if (!path_var_exist(env))
		return (cmd_error("env", NULL, "No such file or directory"), 1);
	tmp = env;
	while (tmp)
	{
		printf("%s \n", tmp->value);
		tmp = tmp->next;
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
