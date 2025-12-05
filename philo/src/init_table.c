/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_table.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 21:15:45 by mishimod          #+#    #+#             */
/*   Updated: 2025/11/27 21:46:31 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// void *thread_func(void* arg)
// {
// 		(void)arg;
// 		printf("hello world\n");
// 		// eating
// 		// sleeping
// 		// thinking
// 		// waiting
// 
// 		return NULL;
// }

static long        get_current_time(void)
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
		i++;
	}

	arg->start_time = get_current_time();

	return ;
}






