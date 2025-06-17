
#include "../exec.h"
#include <string.h>
#include <unistd.h>

int	home_env_var(t_env *env)
{
	while (env)
	{
		if (!ft_strncmp(env->value, "HOME=", 5))
			return (1);
		env = env->next;
	}
	return (0);
}
int	my_cd(t_env *env, char **args)
{
	char	*path;

	if (!(args + 1) && *(args + 1))
		return (cmd_error("cd", NULL, "too many arguments"), 1);
	if (!args)
		path = "/home";
	else
		path = *args;
	if (!ft_strncmp(path, "/home", 5) && !home_env_var(env))
		return (cmd_error("cd", NULL, "HOME not set"), 1);
	if (chdir(path))
		return (cmd_error("cd", path, strerror(errno)), 1);
  //set PWD var to cwd
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	t_shell	*shell;
	char	**args;

	shell = malloc(sizeof(t_shell));
	copy_env(envp, &shell->env);
	args = malloc(16);
	args[0] = "unset";
	args[1] = "HOME";
//	my_unset(&shell->env, args + 1);
	my_cd(shell->env, av + 1);
	my_pwd();
}
