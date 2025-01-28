/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/28 15:24:21 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static void	send_char(char **argv, pid_t pid)
{
	int	i;
	int	bit;
	int	char_num;

	i = 0;
	while (argv[2][i])
	{
		bit = 0;
		while (bit < 8) // per one bit
		{
			if ((argv[2][i] >> bit) & 1)       // reconsider bit caluculation
				char_num = kill(pid, SIGUSR1); // bit 1
			else
				char_num = kill(pid, SIGUSR2); // bit 0
			if (num == -1)                     // failure kill();
			{
				ft_printf("Error\n");
				return (1);
			}
			usleep(500);
			bit++;
		}
		i++;
	}
}

int	main(int argc, const char **argv)
{
	pid_t	pid;

	if (argc != 3)
	{
		ft_printf("Error\n");
		return (1);
	}
	pid = ft_atoi(argv[1]); // overflow
	if (!pid)
		return (1);
    send_char(argv, pid);
	return (0);
}

