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

int	is_builtin(char *cmd)
{
	if (!ft_strcmp(cmd, "exit") || !ft_strcmp(cmd, "unset") || !ft_strcmp(cmd,
			"env") || !ft_strcmp(cmd, "export") || !ft_strcmp(cmd, "cd")
		|| !ft_strcmp(cmd, "pwd") || !ft_strcmp(cmd, "echo"))
		return (1);
	return (0);
}

void	start_exec(t_shell *shell)
{
	if (is_builtin(shell->cmd->args[0]))
		shell->last_exit_status = execute_builtin(shell, shell->cmd);
}

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;

	(void)ac;
	(void)av;

	memset(&shell, 0, sizeof(t_shell));
	copy_env(envp, &shell.env);
	while (1337)
	{
		shell.r_line = readline("Minishell  > ");
		if (shell.r_line == NULL)
			exit(0);
		shell.cmd = malloc(sizeof(t_command));
		shell.cmd->args = ft_split(shell.r_line, ' ');
		if (ft_strlen(shell.r_line))
		{
			add_history(shell.r_line);
			start_exec(&shell);
			printf("exit status : %d \n", shell.last_exit_status);
		}
	}
}
