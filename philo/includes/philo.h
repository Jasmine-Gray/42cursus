/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 19:26:43 by mishimod          #+#    #+#             */
/*   Updated: 2025/09/05 19:07:30 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <errno.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

int		check_args(int argc, char **argv);
void	error_phrase(void);

// utils
void	ft_putstr_fd(char *str, int fd);
int		ft_atoi(char *str);
int		ft_isdigit(int c);

#endif
