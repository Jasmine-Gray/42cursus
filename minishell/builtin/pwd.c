/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:29:55 by mishimod          #+#    #+#             */
/*   Updated: 2025/06/04 22:15:48 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_pwd(void)
{
	char	*pwd_path;

	pwd_path = getcwd(NULL, 0);
	if (pwd_path == NULL)
	{
		perror("getcwd");
		return (1);
	}
	ft_printf("Current working directory: %s\n", pwd_path);
	free(pwd_path);
	return (0);
}

// int	main(void)
// {
// 	return (get_pwd());
// }
