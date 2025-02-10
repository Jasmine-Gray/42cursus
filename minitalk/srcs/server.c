/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:32 by mishimod          #+#    #+#             */
/*   Updated: 2025/02/10 20:34:50 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static int	rxBuffer;

static void	handle_signal(int signum, siginfo_t *info, void *context)
{
	int	result;

	(void)context;
	rxBuffer = (rxBuffer << 1) | (signum - SIGUSR1);
	if (rxBuffer & (1 << 8))
	{
		result = write(1, &rxBuffer, 1);
		rxBuffer = 1;
		if (result == -1)
		{
			kill(info->si_pid, SIGUSR2);
			return ;
		}
	}
	kill(info->si_pid, SIGUSR1);
}

int	main(void)
{
	struct sigaction	sa;

	rxBuffer = 0;
	sa.sa_sigaction = handle_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	ft_printf("PID:%d\n", getpid());
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	while (1)
		pause();
	return (0);
}
