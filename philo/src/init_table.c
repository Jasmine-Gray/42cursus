/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_table.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 21:15:45 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/20 22:45:05 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long        get_current_time(void)
{
        struct timeval        tv;

		if(gettimeofday(&tv, NULL) == -1)
				return (-1);
        return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static int alloc_resources(t_arg *arg)
{
		arg->forks = malloc(sizeof(pthread_mutex_t) * arg->num_of_philo);
		if(!arg->forks)
				return(1);
		arg->philos = malloc(sizeof(t_philo) * arg->num_of_philo);
		if(!arg->philos)
		{
				free(arg->forks);
				return (1);
		}

		if (pthread_mutex_init(&arg->write_lock, NULL) != 0 || 
		    pthread_mutex_init(&arg->dead_lock, NULL) != 0 || 
			pthread_mutex_init(&arg->meal_lock, NULL) != 0)
		{
				free(arg->forks);
				free(arg->philos);
				return (1);
		}
		return (0);
}

int	init_table(t_arg *arg)
{
	int	i;

	if (alloc_resources(arg) != 0)
			return(1);

	i = 0;
	while (i < arg->num_of_philo)
	{
		if(pthread_mutex_init(&arg->forks[i], NULL) != 0)
				return (1);

		arg->philos[i].id = i + 1;
		arg->philos[i].eat_count = 0;
		arg->philos[i].last_meal_time = get_current_time();
		arg->philos[i].arg = arg;

		arg->philos[i].left_fork_id = i;
		arg->philos[i].right_fork_id = (i + 1) % arg->num_of_philo;
		i++;
	}

	arg->start_time = get_current_time();

	return (0);
	

}




