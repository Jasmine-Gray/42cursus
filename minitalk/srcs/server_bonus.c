/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 16:55:20 by mishimod          #+#    #+#             */
/*   Updated: 2025/02/08 16:55:26 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static void	handle_signal(int signum, siginfo_t *info, void *context)
{
	static int	i;
	static int	c;

	(void)context;
	i++;
	if (signum == SIGUSR1)
		c = (c << 1) + 1;
	else
		c = (c << 1) + 0;
	if (i == 8)
	{
		if (c == 0)
			kill(info->si_pid, SIGUSR1);
		else
			write(1, &c, 1);
		i = 0;
		c = 0;
	}
}

int	main(void)
{
	struct sigaction	sa;

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
