
//#include "exec.h"
#include "exec.h"
#include <sys/wait.h>
#include <unistd.h>
#include "string.h"

char	*ft_strjoin_2(char const *s1, char const *s2)
{
	char	*new_str;
	size_t	i;
	size_t	j;
	size_t	s_len1;
	size_t	s_len2;

	s_len1 = strlen(s1);
	s_len2 = strlen(s2);
	new_str = malloc(s_len1 + s_len2 + 1);
	if (!new_str)
		return (NULL);
	i = 0;
	while (i < s_len1)
	{
		new_str[i] = s1[i];
		i++;
	}
	j = 0;
	while (j < s_len2)
	{
		new_str[j + i] = s2[j];
		j++;
	}
	new_str[i + j] = '\0';
	return (new_str);
}

void	input_redirection(int *fd)
{
  dup2(fd[0], 1);
  close(fd[1]);
}

void	output_redirection(int *fd)
{
  dup2(fd[1], 0);
  close(fd[0]);
}

void	exec_child(char **envp, char *av)
{
  char **args = ft_split(av, ' ');
  char *path = ft_strjoin_2("/usr/bin", args[0]);
  execve(path, args, envp);
  exit (127);

}

// ./aout  a b c
int	main(int ac, char **av, char **envp)
{
	int	n_cmd;
	int	n_pip;
	int	tmp;
  int tmp2;
  int i;

  i = -1;
	n_cmd = ac - 1;
	tmp = n_cmd;
	n_pip = n_cmd - 1;
  tmp2 = n_pip;
	int	fd[n_pip][2];
	int	pid[n_cmd];
	while (n_pip--)
	{
		if (pipe(fd[n_pip]))
			return (-1);
	}
  n_pip = tmp;
	while (++i < n_cmd)
	{
		pid[n_cmd] = fork();
    if (!pid[n_cmd])
    {
	    if (i <= n_pip)
	    {
	      if (i)
	        input_redirection(fd[i - 1]);
	      if (i < n_cmd - 1)
	        output_redirection(fd[i]);
	    }
			if (!pid[n_cmd])
				exec_child(envp, av[i + 1]);
		}
  }
	while (tmp--)
  {		
    wait(NULL);
    if (tmp)
    {
      close(fd[tmp-1][0]);
      close(fd[tmp-1][1]);
    }
  }
}
