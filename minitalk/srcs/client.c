/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/02/12 17:39:05 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static volatile sig_atomic_t	g_ack_received;

static void	send_char(const char **argv, pid_t pid)
{
		int	i;
		int	bit;

		i = 0;
		while (argv[2][i])
		{
				bit = 7;
				while (bit >= 0)
				{
						if (argv[2][i] & (1 << bit))
								kill(pid, SIGUSR1);//bit==1
						else
								kill(pid, SIGUSR2);//bit==0
						while (!g_ack_received)
								;
						g_ack_received = 0;
						usleep(500);
						bit--;
				}
				i++;
		}
		bit = 7;
		while(bit >=0)
		{
				kill(pid, SIGUSR2);
				while(!g_ack_received)
						;
				g_ack_received = 0;
				usleep(500);
				bit--;
		}
}

void	handle_ack(int signum)
{
		g_ack_received = signum;
}

int	main(int argc, const char **argv)
{
		pid_t				pid;

		if (argc != 3)
		{
				ft_printf("Error\n");
				return (1);
		}
		pid = ft_atoi(argv[1]);
		if (!pid)
				return (1);
		if (pid < 3)
		{
				ft_printf("Error\n");
				return (1);
		}
		signal(SIGUSR1, handle_ack);
		signal(SIGUSR2, handle_ack);
		g_ack_received = 0;
		send_char(argv, pid);
		return (0);
}
