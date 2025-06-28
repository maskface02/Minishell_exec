/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 18:14:46 by zatais            #+#    #+#             */
/*   Updated: 2025/06/19 18:14:46 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"

void	set_var(t_shell *shell)
{
	t_env	*node;
	char	*eq;

	node = find_env_var(shell->env, "OLDPWD");
	if ()
}

int	my_cd(t_shell *shell, char **args)
{
	char	*path;
	char	*old_path;
	char	*pwd;

	old_path = getcwd(NULL, 0);
	if (*args && *(args + 1))
		return (cmd_error("cd", NULL, "too many arguments"), 1);
	if (!*args)
		path = "/home";
	else
		path = *args;
	if (!ft_strncmp(path, "/home", 5) && !is_var_exist(shell->env, "HOME="))
		return (cmd_error("cd", NULL, "HOME not set"), 1);
	if (chdir(path))
		return (cmd_error("cd", path, strerror(errno)), 1);
	if (shell->cwd)
		free(shell->cwd);
	shell->cwd = getcwd(NULL, 0);
	set_var(shell, "PWD", "OLDPWD", old_path);
	// set PWD var to cwd with export
	// update the oldpwd to old_path if it exist in env
	// free old_path
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	t_shell	*shell;
	char	**args;

	shell = malloc(sizeof(t_shell));
	shell->cwd = getcwd(NULL, 0);
	copy_env(envp, &shell->env);
	args = malloc(16);
	args[0] = "unset";
	args[1] = "HOME";
	//	my_unset(&shell->env, args + 1);
	// my_cd(shell, &av[1]);
	my_pwd(shell);
}
