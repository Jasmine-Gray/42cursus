/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 17:05:10 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/10 17:59:24 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	error_phrase(void)
{
	ft_putstr_fd("USAGE:\n"
					"./philo num_of_philos time_of_philos time_to_die time_to_eat "
					"time_to_sleep [num_of_times_each_philos_must_eat]\n"
					"\n"
					"DESCRIPTION:\n"
					"	- num_of_philos is valid between 1 to MAX_PHILO.\n"
					"		(Default value for MAX_PHILO is 200.)\n"
					"	- num_of_times_each_philos_must_eat must be non zero value.\n"
					"	  when it is negative or not specified the simulation\n"
					"     does not end until someone died.\n",
					STDERR_FILENO);
}
