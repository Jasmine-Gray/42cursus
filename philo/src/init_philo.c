/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_philo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 16:06:59 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/21 21:33:59 by mishimod         ###   ########.fr       */
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

static void	*philo_solo(t_philo *philo)
{
	pthread_mutex_lock(&philo->arg->forks[philo->left_fork_id]);
	print_action(philo, "has taken a fork");
	ft_usleep(philo->arg->time_to_die + 200, philo->arg);
	pthread_mutex_unlock(&philo->arg->forks[philo->left_fork_id]);
	return (NULL);
}

static void	philo_eat(t_philo *philo)
{
	int	f1;
	int	f2;

	f1 = philo->left_fork_id;
	f2 = philo->right_fork_id;
	if (philo->left_fork_id > philo->right_fork_id)
	{
		f1 = philo->right_fork_id;
		f2 = philo->left_fork_id;
	}
	pthread_mutex_lock(&philo->arg->forks[f1]);
	print_action(philo, "has taken a fork");
	pthread_mutex_lock(&philo->arg->forks[f2]);
	print_action(philo, "has taken a fork");
	print_action(philo, "is eating");
	pthread_mutex_lock(&philo->arg->meal_lock);
	philo->last_meal_time = get_current_time();
	pthread_mutex_unlock(&philo->arg->meal_lock);
	ft_usleep(philo->arg->time_to_eat, philo->arg);
	pthread_mutex_unlock(&philo->arg->forks[f2]);
	pthread_mutex_unlock(&philo->arg->forks[f1]);
}

void	*thread_func(void *ptr)
{
	t_philo	*philo;

	philo = (t_philo *)ptr;
	if (philo->arg->num_of_philo == 1)
		return (philo_solo(philo));
	if (philo->id % 2 == 0)
		ft_usleep(philo->arg->time_to_eat / 2, philo->arg);
	while (!check_if_dead(philo->arg))
	{
		philo_eat(philo);
		print_action(philo, "is sleeping");
		pthread_mutex_lock(&philo->arg->meal_lock);
		philo->eat_count++;
		pthread_mutex_unlock(&philo->arg->meal_lock);
		if (philo->arg->num_times_philo_must_eat != -1
			&& philo->eat_count >= philo->arg->num_times_philo_must_eat)
			break ;
		ft_usleep(philo->arg->time_to_sleep, philo->arg);
		print_action(philo, "is thinking");
		if (philo->arg->num_of_philo % 2 != 0)
			ft_usleep((philo->arg->time_to_eat * 2
					- philo->arg->time_to_sleep) * 0.4, philo->arg);
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
