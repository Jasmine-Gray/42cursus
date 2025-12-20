/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 12:41:39 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/20 23:14:01 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include "philo.h"

void    destroy_all(t_arg *arg)
{
    int i;

    i = 0;
    while (i < arg->num_of_philo)
    {
        if (arg->forks)
            pthread_mutex_destroy(&arg->forks[i]);
        i++;
    }
    pthread_mutex_destroy(&arg->write_lock);
    pthread_mutex_destroy(&arg->dead_lock);
    pthread_mutex_destroy(&arg->meal_lock);

    if (arg->forks)
        free(arg->forks);
    if (arg->philos)
        free(arg->philos);
}

void    wait_threads(t_arg *arg)
{
    int i;

    i = 0;
    while (i < arg->num_of_philo)
    {
        pthread_join(arg->philos[i].thread, NULL);
        i++;
    }
}

int main(int argc, char **argv)
{
    t_arg   arg;

    if (check_args(argc, argv) < 0)
        return (1);

    assign_args(argc, argv, &arg);

    if (!check_valid_assign(argc, &arg))
    {
        error_phrase();
        return (1);
    }

    if (init_table(&arg) != 0)
        return (1);

    if (create_thread(&arg) != 0)
    {
        destroy_all(&arg);
        return (1);
    }
    monitor_all(&arg);
    wait_threads(&arg);
    destroy_all(&arg);

    return (0);
}
