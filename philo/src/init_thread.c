/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_thread.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 19:17:51 by mishimod          #+#    #+#             */
/*   Updated: 2025/09/08 18:45:49 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void thread_func(t_arg, *arg)
{
	
}

void	init_thread(t_arg *arg)
{
	int	i;

	i = 0;
	while (i < arg->num_of_philo)
	{
		pthread_create(&arg->philo[i].thread, NULL, thread_func,
			&arg->philo[i]);
		i++;
	}
	return ;
}
