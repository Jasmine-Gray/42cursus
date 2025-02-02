/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:32 by mishimod          #+#    #+#             */
/*   Updated: 2025/02/02 15:13:38 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

int main(void)
{
		ft_printf("PID:%d\n", getpid());
		signal(SIGUSR1, sigaction);
		signal(SIGUSR2, sigaction);
		while(1)
				pause();	
		return (0);
}
