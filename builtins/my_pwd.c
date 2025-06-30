/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zatais <zatais@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 18:49:23 by zatais            #+#    #+#             */
/*   Updated: 2025/06/19 18:15:07 by zatais           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../exec.h"

int	my_pwd(t_shell *shell)
{
	char	buff[4096];

	if (!getcwd(buff, sizeof(buff)))
		return (ft_putendl_fd(shell->cwd, 1), 0);
	return (ft_putendl_fd(buff, 1), 0);
}

/*int main()
{
  my_pwd();
}*/
