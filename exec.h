#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <readline/history.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROMPT "minishell"

#define REDIR_IN 1
#define REDIR_OUT 2
#define REDIR_APPEND 3
#define REDIR_HEREDOC 4

typedef struct s_gc_node
{
	void				*ptr;
	struct s_gc_node	*next;
}						t_gc_node;

typedef struct s_redir
{
	int					type;
	char				*target;
	char				*heredoc_file_name;
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
}						t_env;

typedef struct s_shell
{
	char				*r_line;
	//	t_token     *tokens;
	t_command			*cmd;
	t_env				*env;
	int					last_exit_status;
	char				*cwd;
	t_gc_node			*gc;
}						t_shell;

size_t					ft_strlen(char *s);
void					ft_putstr_fd(char *s, int fd);
int						my_echo(char **args);
void					ft_putendl_fd(char *s, int fd);
void					cmd_error(char *cmd_name, char *error_arg,
							char *error_msg);
int						args_counter(char **args);
long					ft_atol(char *arg, int *overflow);
int						is_space(char c);
int						ft_strncmp(char *s1, char *s2, size_t n);
size_t					ft_strlen(char *s);
int						copy_env(char **envp, t_env **env, t_gc_node **gc);
t_env					*last_node(t_env *node);
void					add_back(t_env **env, t_env *new);
int						my_echo(char **args);
int						my_pwd(t_shell *shell);
int						my_exit(char **args, t_shell *shell);
int						my_unset(t_env **env, char **args, t_gc_node **gc);
int						is_var_exist(t_env *env, char *var);
int						ft_strcmp(char *s1, char *s2);
char					*ft_strndup(char *s, size_t n, t_gc_node **gc);
char					*ft_strchr(char *s, int c);
int						ft_isalnum(int c);
int						ft_isalpha(int c);
int						my_env(t_env *env, char **args);
t_env					*find_env_var(t_env *env, char *name);
int						my_cd(t_shell *shell, char **args, t_gc_node **gc);
char					**ft_split(char *s, char c, t_gc_node **gc);
int						my_export(t_env **env, char **args, t_gc_node **gc);
char					*ft_strdup(char *s, t_gc_node **gc);
void					*gc_malloc(t_gc_node **gc, size_t size);
void					gc_clean(t_gc_node **gc);
void					gc_remove(t_gc_node **gc, void *ptr);
void					gc_add(t_gc_node **gc, void *ptr);
char					*ft_strjoin(char *s1, char *s2, t_gc_node **gc);
t_env					*create_node(char *envp, t_gc_node **gc);
void					start_exec(t_shell *shell);
char          *get_env_value(t_env *env, char *key);

