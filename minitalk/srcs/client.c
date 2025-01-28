/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/27 17:16:18 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

void send_char(const pid_t pid, char *str)
{
		int
		kill(pid, SIGUSR1);
}

int	main(int argc, const char **argv)
{
	pid_t	pid;
	int i;
	if (argc != 3)
	{
		ft_printf("Error\n");
		return (1);
	}

	pid = ft_atoi(argv[1]); // overflow
	if (!pid)
		return (1);
	i = 0;
	while(argv[2][i] != NULL)
	{
			send_char(pid, argv[2][i]);
			i++;
	}
	return (0);
}
