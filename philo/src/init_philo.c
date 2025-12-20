/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_philo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 16:06:59 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/20 23:04:44 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_action(t_philo *philo, char *str)
{
	long	time;

	pthread_mutex_lock(&philo->arg->write_lock);
	if (!check_if_dead(philo->arg) || str[0] == 'd')
	{
		time = get_current_time() - philo->arg->start_time;
		printf("%ld %d %s\n", time, philo->id, str);
	}

	pthread_mutex_unlock(&philo->arg->write_lock);
}

void	*thread_func(void *ptr)
{
	t_philo	*philo;
	t_arg	*arg;

	philo = (t_philo *)ptr;
	arg = philo->arg;
	if (philo->id % 2 == 0)
		ft_usleep(10, arg);
	while (!check_if_dead(arg))
	{
		pthread_mutex_lock(&arg->forks[philo->left_fork_id]);
		print_action(philo, "has taken a fork");
		if (arg->num_of_philo == 1)
		{
			pthread_mutex_unlock(&arg->forks[philo->left_fork_id]);
			ft_usleep(arg->time_to_die + 200, arg);
			return (NULL);
		}
		pthread_mutex_lock(&arg->forks[philo->right_fork_id]);
		print_action(philo, "has taken a fork");
		print_action(philo, "is eating");
		pthread_mutex_lock(&arg->meal_lock);
		philo->last_meal_time = get_current_time();
		philo->eat_count++;
		pthread_mutex_unlock(&arg->meal_lock);
		ft_usleep(arg->time_to_eat, arg);
		pthread_mutex_unlock(&arg->forks[philo->right_fork_id]);
		pthread_mutex_unlock(&arg->forks[philo->left_fork_id]);
		if (arg->num_times_philo_must_eat != -1
			&& philo->eat_count >= arg->num_times_philo_must_eat)
			break ;
		print_action(philo, "is sleeping");
		ft_usleep(arg->time_to_sleep, arg);
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
		if (pthread_create(&arg->philos[i].thread, NULL, thread_func,
				&arg->philos[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}
