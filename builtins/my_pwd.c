/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 18:49:23 by zatais            #+#    #+#             */
/*   Updated: 2025/06/13 18:49:23 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"

int	my_pwd()
{
	char	buff[4096];

	if (getcwd(buff, sizeof(buff)))
    return (ft_putendl_fd(buff, 1), 1);
	//cmd_error("pwd", /*strerror(errno)*/"error retrieving current directory");
	return (0);
}

/*int main()
{
  my_pwd();
}*/
