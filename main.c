#include "exec.h"

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;

	(void)ac;
	(void)av;

	memset(&shell, 0, sizeof(t_shell));
	copy_env(envp, &shell.env, &shell.gc);
	while (1337)
	{
		shell.r_line = readline("Minishell  > ");
		if (shell.r_line == NULL)
			exit(0);
		shell.cmd = malloc(sizeof(t_command));
    shell.cmd->redirs = NULL;
		shell.cmd->args = ft_split(shell.r_line, ' ', &shell.gc);
    shell.cmd->next = NULL;
		if (ft_strlen(shell.r_line))
		{
			add_history(shell.r_line);
			start_exec(&shell);
			printf("exit status : %d \n", shell.last_exit_status);
      free(shell.r_line);
		}
	}
}
