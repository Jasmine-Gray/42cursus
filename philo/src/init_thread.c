/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_thread.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 19:17:51 by mishimod          #+#    #+#             */
/*   Updated: 2025/09/08 17:24:53 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

thread_func()
{
	
}

void	init_thread(t_thread *thread)
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
