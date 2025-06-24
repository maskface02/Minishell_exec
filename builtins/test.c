#include "../exec.h"

int	compare_env_vars(char *s1, char *s2, int *error)
{
	char	*eq1;
	char	*eq2;
	int		len2;
	int		len1;
	int		i;
	int		min_len;

	eq1 = ft_strchr(s1, '=');
	if (eq1)
		len1 = eq1 - s1;
	else
		len1 = ft_strlen(s1);
	eq2 = ft_strchr(s2, '=');
	if (eq2)
		len2 = eq2 - s2;
	else
		len2 = ft_strlen(s2);
	min_len = len1;
	if (len2 < min_len)
		min_len = len2;
	i = 0;
	while (i <= min_len)
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

int main()
{
  int ok = compare_env_vars("LANG=en_US.UTF-8", "LANGUAGE=en", NULL);
}
