/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 23:06:51 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/20 23:07:10 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void ft_usleep(long time_in_ms, t_arg *arg)
{
    long start_time;

    start_time = get_current_time();
    while ((get_current_time() - start_time) < time_in_ms)
    {
        if (check_if_dead(arg))
            break;
        usleep(500); 
    }
}
