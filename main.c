#include "exec.h"

int	execute_builtin(t_shell *shell, t_command *cmd)
{
	int	ret;

	ret = 127;
	if (!ft_strncmp(cmd->args[0], "cd", 3))
		ret = my_cd(shell, &cmd->args[1]);
	else if (!ft_strncmp(cmd->args[0], "echo", 5))
		ret = my_echo(&cmd->args[1]);
	else if (!ft_strncmp(cmd->args[0], "env", 4))
		ret = my_env(shell->env, &cmd->args[1]);
	else if (!ft_strncmp(cmd->args[0], "export", 7))
		ret = my_export(&shell->env, &cmd->args[1]);
	else if (!ft_strncmp(cmd->args[0], "pwd", 4))
		ret = my_pwd(shell);
	else if (!ft_strncmp(cmd->args[0], "unset", 6))
		ret = my_unset(&shell->env, &cmd->args[1]);
	else if (!ft_strncmp(cmd->args[0], "exit", 5))
		ret = my_exit(&cmd->args[1], shell);
	return (ret);
}

int	main(int ac, char **av, char **env)
{
	t_shell	shell;

	(void)ac;
	(void)av;
	(void)env;
	while (1337)
	{
    memset(&shell, 0, sizeof(t_shell));
		shell.r_line = readline("Minishell  > ");
		if (shell.r_line == NULL)
			exit(0);
		if (ft_strlen(shell.r_line) != 0)
		{
			add_history(shell.r_line);
      start_exec(shell);
		}
	}
}
