/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assign_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 19:20:01 by mishimod          #+#    #+#             */
/*   Updated: 2025/09/08 15:04:05 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	assign_args(int argc, char **argv, t_arg *arg)
{
	arg->num_of_philo = ft_atoi(argv[1]);
	arg->time_to_die = ft_atoi(argv[2]);
	arg->time_to_eat = ft_atoi(argv[3]);
	arg->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		arg->num_times_philo_must_eat = ft_atoi(argv[5]);
	else
		arg->num_times_philo_must_eat = INT_MAX;
}

int	check_valid_assign(int argc, t_arg *arg)
{
	if (arg->num_of_philo <= 0 || arg->num_of_philo > INT_MAX)
		return (0);
	if (arg->time_to_die <= 0 || arg->time_to_die > INT_MAX)
		return (0);
	if (arg->time_to_eat <= 0 || arg->time_to_eat > INT_MAX)
		return (0);
	if (arg->time_to_sleep <= 0 || arg->time_to_sleep > INT_MAX)
		return (0);
	if (argc == 6 && (arg->num_times_philo_must_eat <= 0
			|| arg->num_times_philo_must_eat > INT_MAX))
		return (0);
	return (1);
}
