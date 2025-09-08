/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 19:26:43 by mishimod          #+#    #+#             */
/*   Updated: 2025/09/08 16:41:07 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <errno.h>
# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define MAX_PHILO 200

typedef struct s_philo
{
	struct s_arg	*arg;
	p_thread		*thread;
}					t_philo;

typedef struct s_arg
{
	int				num_of_philo;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_times_philo_must_eat;
	t_philo			philo[MAX_PHILO];
}					t_arg;

// srcs
int					check_args(int argc, char **argv);
void				assign_args(int argc, char **argv, t_arg *arg);
int					check_valid_assign(int argc, t_arg *arg);
void				error_phrase(void);

// utils
void				ft_putstr_fd(char *str, int fd);
int					ft_atoi(char *str);
int					ft_isdigit(int c);

#endif
