/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/02/03 18:25:21 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static int send_null(pid_t pid)
{
		int bit;
		int char_num;

		bit = 0;
		while(bit < 8)
		{
				char_num = kill(pid, SIGUSR2);
				if (char_num == -1)
				{
						ft_printf("Error\n");
						return (1);
				}
				usleep(500);
				bit++;
		}
		return (0);
}

static int	send_char(const char **argv, pid_t pid)
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
			if ((argv[2][i] >> (7 - bit)) & 1)       // reconsider bit caluculation
				char_num = kill(pid, SIGUSR1); // bit 1
			else
				char_num = kill(pid, SIGUSR2); // bit 0
			if (char_num == -1)                     // failure kill();
			{
				ft_printf("Error\n");
				return (1);
			}
			usleep(500);
			bit++;
		}
		i++;
	}
	return (send_null(pid));
}

void handle_ack(int signum)
{
		if (signum == SIGUSR1)
				ft_printf("acknowledgement received from server\n");
}

int	main(int argc, const char **argv)
{
	pid_t	pid;
	struct sigaction sa;

	if (argc != 3)
	{
		ft_printf("Error\n");
		return (1);
	}
	pid = ft_atoi(argv[1]); // overflow
	if (!pid)
		return (1);
	if (pid < 3)
	{
		ft_printf("Error\n");
		return (1);
	}
	 sa.sa_handler = handle_ack;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    send_char(argv, pid);
	return (0);
}

