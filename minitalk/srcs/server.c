/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:32 by mishimod          #+#    #+#             */
/*   Updated: 2025/02/02 16:19:04 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static void handle_signal(int signum)
{
		static int i;
		static int c;

		i++;
		if (signum == SIGUSR1)
				c = (c << 1) + 1;
		else
				c = (c << 1) + 0;
		if (i == 8)
		{
				write(1, &c, 1);
				i = 0;
				c = 0;
		}
}

int	main(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	ft_printf("PID:%d\n", getpid());
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	while (1)
		pause();
	return (0);
}
