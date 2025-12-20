/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 23:05:43 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/20 23:06:18 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int check_if_dead(t_arg *arg)
{
    int ret;

    pthread_mutex_lock(&arg->dead_lock);
    ret = arg->is_dead;
    pthread_mutex_unlock(&arg->dead_lock);
    return (ret);
}

int check_must_eat(t_arg *arg)
{
    int i;
    int finished_eating;

    if (arg->num_times_philo_must_eat == -1)
        return (0);

    finished_eating = 0;
    i = 0;
    while (i < arg->num_of_philo)
    {
        pthread_mutex_lock(&arg->meal_lock);
        if (arg->philos[i].eat_count >= arg->num_times_philo_must_eat)
            finished_eating++;
        pthread_mutex_unlock(&arg->meal_lock);
        i++;
    }
    if (finished_eating == arg->num_of_philo)
    {
        pthread_mutex_lock(&arg->dead_lock);
        arg->is_dead = 1;
        pthread_mutex_unlock(&arg->dead_lock);
        return (1);
    }
    return (0);
}

void monitor_all(t_arg *arg)
{
    int i;

    while (1)
    {
        i = 0;
        while (i < arg->num_of_philo)
        {
            pthread_mutex_lock(&arg->meal_lock);
            if (get_current_time() - arg->philos[i].last_meal_time >= arg->time_to_die)
            {
                pthread_mutex_unlock(&arg->meal_lock);
                print_action(&arg->philos[i], "died");
                
                pthread_mutex_lock(&arg->dead_lock);
                arg->is_dead = 1;
                pthread_mutex_unlock(&arg->dead_lock);
                return ;
            }
            pthread_mutex_unlock(&arg->meal_lock);
            i++;
        }
        if (check_if_dead(arg) || check_must_eat(arg))
            return ;
        usleep(1000); 
    }
}
