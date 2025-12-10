/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_philo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 16:06:59 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/10 17:58:59 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_action(t_philo *philo, char *str)
{
	long	time;

	time = get_current_time() - philo->arg->start_time;
	printf("%ld %d %s\n", time, philo->id, str);
}

void	*thread_func(void *ptr)
{
	t_philo	*philo;
	t_arg	*arg;
	int		left_fork;
	int		right_fork;

	philo = (t_philo *)ptr;
	arg = philo->arg;
	left_fork = philo->id - 1;
	right_fork = philo->id % arg->num_of_philo;
	if (philo->id % 2 == 0)
		usleep(1500);
	while (1)
	{
		pthread_mutex_lock(&arg->fork[left_fork]);
		print_action(philo, "has taken a fork");
		if (arg->num_of_philo == 1)
		{
			pthread_mutex_unlock(&arg->fork[left_fork]);
			return (NULL);
		}
		pthread_mutex_lock(&arg->fork[right_fork]);
		print_action(philo, "has taken a fork");
		print_action(philo, "is eating");
		philo->last_meal_time = get_current_time();
		usleep(arg->time_to_eat * 1000);
		philo->eat_count++;
		pthread_mutex_unlock(&arg->fork[left_fork]);
		pthread_mutex_unlock(&arg->fork[right_fork]);
		print_action(philo, "is sleeping");
		usleep(arg->time_to_sleep * 1000);
		print_action(philo, "is thinking");
	}
	return (NULL);
}

int	create_thread(t_arg *arg)
{
	int	i;

	i = 0;
	while (i < arg->num_of_philo)
	{
		if (pthread_create(&arg->philo[i].thread, NULL, thread_func,
				&arg->philo[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}
