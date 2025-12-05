/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 19:26:43 by mishimod          #+#    #+#             */
/*   Updated: 2025/11/27 21:43:50 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# define MAX_PHILO 200

#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct s_philo
{
	struct s_arg	*arg;
	pthread_t		*thread;
}					t_philo;

typedef struct s_arg
{
	int				num_of_philo;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_times_philo_must_eat;
	pthread_mutex_t	fork[MAX_PHILO];
	long			start_time;
	t_philo			philo[MAX_PHILO];
}					t_arg;

// threads
void				init_table(t_arg *arg);
void				*thread_func(t_arg *arg);

// args
int					check_args(int argc, char **argv);
int					check_valid_assign(int argc, t_arg *arg);
void				assign_args(int argc, char **argv, t_arg *arg);

// utils
void				ft_putstr_fd(char *str, int fd);
int					ft_atoi(char *str);
int					ft_isdigit(int c);
void				error_phrase(void);

#endif
