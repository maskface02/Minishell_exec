#include "exec.h"

t_command *create_command(char **args, t_redir *redirs, t_gc_node *gc) {
    t_command *cmd = gc_malloc(&gc, sizeof(t_command));
    cmd->args = args;
    cmd->redirs = redirs;
    cmd->next = NULL;
    return cmd;
}

// Helper to create environment variables
t_env *create_env_var(char *value, t_gc_node *gc) {
    t_env *env = gc_malloc(&gc, sizeof(t_env));
    env->value = value;
    env->next = NULL;
    return env;
}

int main(int ac, char **av, char **envp) {
    t_gc_node *gc = {0}; // Dummy garbage collector
    t_shell shell = {0};
    shell.gc = gc;
    shell.last_exit_status = 0;

    // Create environment variables
    copy_env(envp, &shell.env, &shell.gc);

    // Create commands for: ls | grep "a" | wc -l
    char *ls_args[] = {"ls", NULL};
    char *grep_args[] = {"grep", "a", NULL};
    char *wc_args[] = {"wc", "-l", NULL};

    t_command *cmd1 = create_command(ls_args, NULL, shell.gc);
    t_command *cmd2 = create_command(grep_args, NULL, shell.gc);
    t_command *cmd3 = create_command(wc_args, NULL, shell.gc);

    // Link commands together
    cmd1->next = cmd2;
    cmd2->next = cmd3;
    shell.cmd = cmd1;

    printf("Testing command pipeline: ls | grep \"a\" | wc -l\n");
    start_exec(&shell);
    printf("Pipeline execution completed. Exit status: %d\n", shell.last_exit_status);

    // Test built-in command (should run in main process)
    printf("\nTesting built-in command:\n");
    t_command *pwd_cmd = create_command((char*[]){"pwd", NULL}, NULL, shell.gc);
    shell.cmd = pwd_cmd;
    start_exec(&shell);
    printf("Built-in executed. Exit status: %d\n", shell.last_exit_status);

    // Clean up (in real implementation use proper gc_clean)
    free(cmd1);
    free(cmd2);
    free(cmd3);
    free(pwd_cmd);

    return 0;
}
