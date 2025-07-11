/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 18:50:14 by zatais            #+#    #+#             */
/*   Updated: 2025/06/13 21:08:27 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	i = 0;
	if (!s)
		return ;
	while (s[i])
	{
		write(fd, &s[i], 1);
		i++;
	}
}

void	ft_putendl_fd(char *s, int fd)
{
	if (!s)
		return ;
	ft_putstr_fd(s, fd);
	write(fd, "\n", 1);
}

size_t	ft_strlen(char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strjoin(char *s1, char *s2, t_gc_node **gc)
{
	size_t	len1;
	size_t	len2;
	char	*res;
	size_t	i;

	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	res = gc_malloc(gc, len1 + len2 + 1);
	i = 0;
	while (*s1)
		res[i++] = *s1++;
	while (*s2)
		res[i++] = *s2++;
	res[i] = '\0';
	return (res);
}
char	*ft_strndup(char *s, size_t n)
{
	char	*res;
	size_t	i;

	i = 0;
	res = malloc(sizeof(n + 1));
	if (!res)
		return (NULL);
	while (i < n)
	{
		res[i] = s[i];
		i++;
	}
	res[i] = '\0';
	return (res);
}
char	*ft_strdup(char *s, t_gc_node **gc)
{
	size_t	len;
	char	*dup;
	size_t	i;

	len = ft_strlen(s) + 1;
	dup = gc_malloc(gc, len);
	i = 0;
	while (s[i])
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}
// norm
void	cmd_error(char *cmd, char *arg, char *msg)
{
	write(2, "minishell: ", 11);
	write(2, cmd, ft_strlen(cmd));
	if (arg)
	{
		write(2, ": ", 2);
		write(2, arg, ft_strlen(arg));
	}
	if (msg)
	{
		write(2, ": ", 2);
		write(2, msg, ft_strlen(msg));
	}
	write(2, "\n", 1);
}

int	is_overflowed(long res, int last_digit, int sign)
{
	long	max_threshold;
	long	min_threshold;
	int		max_last_digit;
	int		min_last_digit;

	max_threshold = LONG_MAX / 10;
	min_threshold = LONG_MIN / 10;
	max_last_digit = 7;
	min_last_digit = -8;
	if (sign == 1 && (res > max_threshold || (res == max_threshold
				&& last_digit > max_last_digit)))
		return (1);
	if (sign == -1 && (res < min_threshold || (res == min_threshold
				&& last_digit > min_last_digit)))
		return (1);
	return (0);
}
int	is_space(char c)
{
	return ((c >= 0 && c <= 13) || c == 32);
}

long	ft_atol(char *arg, int *overflow)
{
	int		sign;
	long	res;
	int		digit;

	while (is_space(*arg))
		arg++;
	sign = 1;
	if (*arg == '-' || *arg == '+')
	{
		if (*arg == '-')
			sign *= -1;
		arg++;
	}
	res = 0;
	while (*arg >= '0' && *arg <= '9')
	{
		digit = *arg - 48;
		if (is_overflowed(res, digit, sign))
			return (*overflow = 1, LONG_MAX);
		res = (res * 10) + digit;
		arg++;
	}
	return (res * sign);
}

/*int main()
{
  int overflowed = 0;
  long val = ft_atol("9223372036854775900", &overflowed);
  if (overflowed)
	printf("long has been overflowed !");
  else
   printf("%ld", val);
}*/
