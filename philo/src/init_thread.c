/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_thread.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 01:01:10 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/06 20:45:21 by mishimod         ###   ########.fr       */
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

int create_thread(t_arg *arg)
{
		int i;

		i = 0;
		while(i < arg->num_of_philo)
		{
				if (pthread_create(&arg->philo[i], NULL, thread_func, &arg->philo[i]) != 0)
						return (1);
				i++;
		}
		return (0);
}
