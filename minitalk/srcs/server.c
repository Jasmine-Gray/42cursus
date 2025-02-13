/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:32 by mishimod          #+#    #+#             */
/*   Updated: 2025/02/13 17:55:45 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static int	g_rxbuffer;

static void	handle_signal(int signum, siginfo_t *info, void *context)
{
	static int	bits_count = 0;
	char		c;
	int			result;

	(void)context;
	g_rxbuffer = (g_rxbuffer << 1) | (signum == SIGUSR1);
	bits_count++;
	if (bits_count == 8)
	{
		c = g_rxbuffer;
		result = write(1, &c, 1);
		if (result == -1)
		{
			kill(info->si_pid, SIGUSR2);
			return ;
		}
		bits_count = 0;
		g_rxbuffer = 0;
	}
	kill(info->si_pid, SIGUSR1);
}

int	main(void)
{
	struct sigaction	sa;

	g_rxbuffer = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handle_signal;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	ft_printf("PID:%d\n", getpid());
	while (1)
		pause();
	return (0);
}
