/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:05:07 by mishimod          #+#    #+#             */
/*   Updated: 2025/08/11 05:07:37 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_handler(int sig)
{
	g_signal_number = sig;
}

int	on_rl_event(void)
{
	static int	handled = 0;

	if (g_signal_number == SIGINT)
	{
		if (!handled)
		{
			rl_replace_line("", 0);
			if (rl_done == 0)
				rl_done = 1;
			handled = 1;
		}
	}
	else if (handled)
	{
		handled = 0;
	}
	return (0);
}
