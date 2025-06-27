#include "../exec.h"

void free_cmd(t_command *cmd)
{
  t_command *tmp;

  while (cmd)
  {
    tmp = cmd;
    cmd = cmd->next;
    free(tmp);
  }
}

void	free_env(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env;
		env = env->next;
    free(tmp->value);
		free(tmp);
	}
}


