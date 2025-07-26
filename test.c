#include "exec.h"

t_command	*create_command(char **args, t_redir *redirs, t_gc_node **gc)
{
	t_command	*cmd;

	cmd = gc_malloc(gc, sizeof(t_command));
	cmd->args = args;
	cmd->redirs = redirs;
	cmd->next = NULL;
	return (cmd);
}

t_env	*create_env_var(char *value, t_gc_node **gc)
{
	t_env	*env;

	env = gc_malloc(gc, sizeof(t_env));
	env->value = value;
	env->next = NULL;
	return (env);
}

int	main(int ac, char **av, char **envp)
{
	t_shell		shell = {0};
	char		*ls_args[] = {"hhhhht", NULL};
	t_command	*cmd1;
	t_command	*pwd_cmd;

	shell.last_exit_status = 0;
	copy_env(envp, &shell.env, &shell.gc);
	char *grep_args[] = {"jdvc", "a", NULL};
	char *wc_args[] = {"echo", "hello world", NULL};
	cmd1 = create_command(ls_args, NULL, &shell.gc);//
	 t_command *cmd2 = create_command(grep_args, NULL, &shell.gc);
	t_command *cmd3 = create_command(wc_args, NULL, &shell.gc);
	cmd1->next = cmd2;
	cmd2->next = cmd3;
	shell.cmd = cmd1;
	start_exec(&shell);
	printf("Pipeline execution completed. Exit status: %d\n",
		shell.last_exit_status);
	pwd_cmd = create_command((char *[]){"pwd", NULL}, NULL, &shell.gc);//
	shell.cmd = pwd_cmd;
	start_exec(&shell);
	printf("Built-in executed. Exit status: %d\n", shell.last_exit_status);
	//free(cmd1);
	// free(cmd2);
	// free(cmd3);
	//free(pwd_cmd);
	gc_clean(&shell.gc);
	return (shell.last_exit_status);
}
