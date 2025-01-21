/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 09:21:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/21 23:10:43 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

void ft_signal(const pid_t pid, char *str)
{
		int 
		kill(pid, SIGUSR1);
}

int main(int argc, const char **argv)
{
		pid_t pid;
		if (argc == 3)
		{
				pid = ft_atoi(argv[1]);//overflow
				if (エラー処理)
				{
				}
				ft_signal(pid, argv[2]);
		}
		else 
				return (1);
		return (0);
}
