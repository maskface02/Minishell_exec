#include "../exec.h"

int count_arguments(char **args)
{
    int count;
    
    count = 0;
    if (!args)
        return 0;
    while (args[count])
        count++;
    return count;
}

char *get_env_value(t_env *env, char *key)
{
    size_t key_len;
    char *val;
    
    key_len = ft_strlen(key);
    while (env)
    {
        if (!ft_strncmp(env->value, key, key_len))
        {
            if (env->value[key_len] == '=' || env->value[key_len] == '\0')
            {
                val = ft_strchr(env->value, '=');
                if (val)
                    return val + 1;
            }
        }
        env = env->next;
    }
    return NULL;
}

char *get_path(t_shell *shell, char **args, int *debug)
{
    int count;
    char *home;
    char *oldpwd;
    
    count = count_arguments(args);
    if (count == 0 || !ft_strcmp(args[0], "~"))
    {
        home = get_env_value(shell->env, "HOME");
        if (!home)
        {
            cmd_error("cd", NULL, "HOME not set");
            return NULL;
        }
        return home;
    }
    if (!ft_strcmp(args[0], "-"))
    {
        oldpwd = get_env_value(shell->env, "OLDPWD");
        if (!oldpwd)
            cmd_error("cd", NULL, "OLDPWD not set");
        *debug = 1;
        return oldpwd;
    }
    return args[0];
}

char *create_export_arg(char *key, char *value)
{
    char *tmp;
    char *res;
    
    tmp = ft_strjoin(key, "=");
    if (!tmp)
        return NULL;
    res = ft_strjoin(tmp, value);
    free(tmp);
    return res;
}

int update_pwd_vars(t_shell *shell, char *oldpwd, char *new)
{
    char *oldpwd_arg;
    char *pwd_arg;
    char *export_args[3];
    
    oldpwd_arg = create_export_arg("OLDPWD", oldpwd);
    if (!oldpwd_arg)
        return 0;
    pwd_arg = create_export_arg("PWD", new);
    if (!pwd_arg)
    {
        free(oldpwd_arg);
        return 0;
    }
    export_args[0] = oldpwd_arg;
    export_args[1] = pwd_arg;
    export_args[2] = NULL;
    my_export(&shell->env, export_args);
    free(oldpwd_arg);
    free(pwd_arg);
    return 1;
}

int update_shell_state(t_shell *shell, char *new_path, int debug)
{
    char *new_cwd;
    
    new_cwd = ft_strdup(new_path);
    if (!new_cwd)
        return 0;
    free(shell->cwd);
    shell->cwd = new_cwd;
    if (debug)
    {
        ft_putstr_fd(shell->cwd, STDOUT_FILENO);
        ft_putstr_fd("\n", STDOUT_FILENO);
    }
    return 1;
}

int my_cd(t_shell *shell, char **args)
{
    char *oldpwd;
    int debug;
    int count;
    char *path;
    char *new;
    int status;
    
    oldpwd = getcwd(NULL, 0);
    if (!oldpwd)
    {
        cmd_error("cd", "error", strerror(errno));
        return 1;
    }
    debug = 0;
    count = count_arguments(args);
    if (count > 1)
    {
        cmd_error("cd", NULL, "too many arguments");
        free(oldpwd);
        return 1;
    }
    path = get_path(shell, args, &debug);
    if (!path)
    {
        free(oldpwd);
        return 1;
    }
    if (chdir(path))
    {
        cmd_error("cd", path, strerror(errno));
        free(oldpwd);
        return 1;
    }
    new = getcwd(NULL, 0);
    if (!new)
        new = ft_strdup(path);
    if (!new)
    {
        free(oldpwd);
        return 1;
    }
    status = 0;
    if (!update_pwd_vars(shell, oldpwd, new))
        status = 1;
    if (!update_shell_state(shell, new, debug))
        status = 1;
    free(oldpwd);
    free(new);
    return status;
}
