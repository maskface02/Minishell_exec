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

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*new_str;
	size_t	i;
	size_t	j;
	size_t	s_len1;
	size_t	s_len2;

	s_len1 = ft_strlen(s1);
	s_len2 = ft_strlen(s2);
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

char	*ft_strdup(char *s)
{
	char	*t;
	size_t	i;

	t = malloc(ft_strlen(s) + 1);
	if (!t)
		return (NULL);
	i = 0;
	while (s[i])
	{
		t[i] = s[i];
		i++;
	}
	t[i] = '\0';
	return (t);
}
// norm
void	cmd_error(char *cmd_name, char *error_arg, char *error_msg)
{
	char	*full_msg;
	char	*tmp;

	tmp = ft_strdup(PROMPT);
	full_msg = ft_strjoin(tmp, ": ");
	free(tmp);
	if (cmd_name)
	{
		tmp = full_msg;
		full_msg = ft_strjoin(tmp, cmd_name);
		free(tmp);
		tmp = full_msg;
		full_msg = ft_strjoin(tmp, ": ");
		free(tmp);
	}
	if (error_arg)
	{
		tmp = full_msg;
		full_msg = ft_strjoin(tmp, error_arg);
		free(tmp);
		tmp = full_msg;
		full_msg = ft_strjoin(tmp, ": ");
		free(tmp);
	}
	tmp = full_msg;
	full_msg = ft_strjoin(tmp, error_msg);
	free(tmp);
	ft_putendl_fd(full_msg, 2);
	free(full_msg);
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
