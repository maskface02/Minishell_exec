#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROMPT "minishell"

typedef struct s_redir
{
	int					type;
	char				*target;
	char				*content;
	struct s_redir		*next;
}						t_redir;

typedef struct s_command
{
	char				**args;
	t_redir				*redirs;
	struct s_command	*next;
	struct s_command	*prev;
}						t_command;

typedef struct s_env
{
	char				*value;
	struct s_env		*next;
	// struct s_env *prev;
}						t_env;

typedef struct s_shell
{
	char				*r_line;
	//	t_token     *tokens;
	t_command			*cmd;
	t_redir				*red;
	t_env				*env;
	int					last_exit_status;
}						t_shell;

void					ft_putstr_fd(char *s, int fd);
int						my_echo(char **args);
int						my_pwd(void);
void					ft_putendl_fd(char *s, int fd);
void					cmd_error(char *cmd_name, char *error_arg,
							char *error_msg);
int						args_counter(char **args);
long					ft_atol(char *arg, int *overflow);
int						is_space(char c);
char					*ft_strdup(char *s);
int						ft_strncmp(char *s1, char *s2, size_t n);
size_t					ft_strlen(char *s);
int						copy_env(char **envp, t_env **env);
t_env					*create_node(char *envp);
t_env					*last_node(t_env *node);
int						add_back(t_env **env, t_env *new);
void					free_env(t_env *env);
void					free_cmd(t_command *cmd);
int						my_echo(char **args);
int						my_pwd(void);
int						my_exit(char **args, t_shell *shell);
int						my_unset(t_env **env, char **args);
