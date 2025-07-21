#include "../exec.h"

static	int	w_counter(char *s, char c)
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

static	int	w_len(char *s, char c)
{
	int	len;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	return (len);
}

static	char	*extract_w(char *s, int len, t_gc_node **gc)
{
	char	*word;
	int		i;

	i = 0;
	word = gc_malloc(gc, len + 1);
	while (i < len)
	{
		word[i] = s[i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

static	char	**fill_res(char *s, char c, char **res, t_gc_node **gc)
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
			res[idx] = extract_w(s, len, gc);
			idx++;
			s += len;
		}
	}
	res[idx] = NULL;
	return (res);
}

char	**ft_split(char *s, char c, t_gc_node **gc)
{
	char	**res;

	if (!s)
		return (NULL);
	res = gc_malloc(gc, sizeof(char *) * (w_counter(s, c) + 1));
	if (!fill_res(s, c, res, gc))
	{
		gc_remove(gc, res);
		return (NULL);
	}
	return (res);
}
