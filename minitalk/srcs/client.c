/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/02/10 20:45:03 by mishimod         ###   ########.fr       */
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
		bit = 0;
		while (bit < 8)
		{
			if ((argv[2][i] >> (7 - bit)) & 1)
				kill(pid, SIGUSR1);//bit==1
			else
				kill(pid, SIGUSR2);//bit==0
			while (!g_ack_received)
				;
			g_ack_received = 0;
			usleep(500);
			bit++;
		}
		i++;
	}
}

// static void	send_null(pid_t pid)
// {
// 	int	bit;
// 
// 	bit = 8;
// 	while (bit--)
// 	{
// 		kill(pid, SIGUSR2);
// 		usleep(100);
// 	}
// }

void	handle_ack(int signum)
{
	g_ack_received = signum;
}

int	main(int argc, const char **argv)
{
	pid_t				pid;
	struct sigaction	sa;

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
	sa.sa_handler = handle_ack;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);
	g_ack_received = 0;
	send_char(argv, pid);
	//send_null(pid);
	return (0);
}
