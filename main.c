#include "exec.h"

int	main(int ac, char **av, char **env)
{
	t_shell	shell;

	(void)ac;
	(void)av;
	(void)env;
	while (1337)
	{
    memset(&shell, 0, sizeof t_shell);
		shell.r_line = readline("Minishell  > ");
		if (shell.r_line == NULL)
			exit(0);
		if (ft_strlen(shell.r_line) != 0)
		{
			add_history(shell.r_line);
      start_exec(shell->);
		}
	}
}
