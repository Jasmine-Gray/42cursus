/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 19:26:43 by mishimod          #+#    #+#             */
/*   Updated: 2025/12/10 21:02:32 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# define MAX_PHILO 200

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

struct s_arg;

typedef struct s_philo
{
	struct s_arg	*arg;

	int				id;
	int				eat_count;
	int				left_fork_id;
	int				right_fork_id;
	long			last_meal_time;
	pthread_t		thread;

}					t_philo;

typedef struct s_arg
{
	int				num_of_philo;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_times__must_eat;
	long			start_time;
	int				is_dead;

	pthread_mutex_t	forks;
	pthread_mutex_t	write_lock;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	meal_lock;
	t_philo			philos;
}					t_arg;

// threads
void				init_table(t_arg *arg);
long				get_current_time(void);
int					create_thread(t_arg *arg);
void				*thread_func(void *arg);
void				print_action(t_philo *philo, char *str);

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
