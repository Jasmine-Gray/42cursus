/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_table.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 21:15:45 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/10 17:44:21 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long        get_current_time(void)
{
        struct timeval        tv;

        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	init_table(t_arg *arg)
{
	int	i;

	i = 0;
	while (i < arg->num_of_philo)
	{
		pthread_mutex_init(&arg->fork[i], NULL);

		arg->philo[i].id = i + 1;
		arg->philo[i].eat_count = 0;
		arg->philo[i].last_meal_time = get_current_time();
		arg->philo[i].arg = arg;
		i++;
	}

	arg->start_time = get_current_time();

	return ;
}




