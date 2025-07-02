#include "exec.h"

static	int	w_counter(char const *s, char c)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i])
			count++;
		while (s[i] && s[i] != c)
			i++;
	}
	return (count);
}

static	int	w_len(char const *s, char c)
{
	int	len;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	return (len);
}

static	char	*extract_w(char const *s, int len)
{
	char	*word;
	int		i;

	i = 0;
	word = malloc(len + 1);
	if (!word)
		return (NULL);
	while (i < len)
	{
		word[i] = s[i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

static	char	**fill_res(char const *s, char c, char **res)
{
	int	idx;
	int	len;

	idx = 0;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s)
		{
			len = w_len(s, c);
			res[idx] = extract_w(s, len);
			if (!res[idx])
			{
				while (idx > 0)
					free(res[--idx]);
				free(res);
				return (NULL);
			}
			idx++;
			s += len;
		}
	}
	res[idx] = NULL;
	return (res);
}

char	**ft_split(char const *s, char c)
{
	char	**res;

	if (!s)
		return (NULL);
	res = malloc(sizeof(char *) * (w_counter(s, c) + 1));
	if (!res)
		return (NULL);
	if (!fill_res(s, c, res))
	{
		free(res);
		return (NULL);
	}
	return (res);
}
