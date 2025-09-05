/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 17:11:10 by mishimod          #+#    #+#             */
/*   Updated: 2025/09/05 19:01:36 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_putstr_fd(char *str, int fd)
{
	int	i;

	i = 0;
	while (str[i])
	{
		write(fd, &str[i], 1);
		i++;
	}
}

static int	ft_isspace(int c)
{
	if (c >= 0 && c <= 32)
		return (1);
	return (0);
}

int	ft_atoi(char *str)
{
	int	number;
	int	i;

	i = 0;
	while (ft_isspace(str[i]))
		i++;
	if (str[i] == '-')
		return (-1);
	number = 0;
	while (str[i] >= 0 && str[i] <= 9)
	{
		number = number * 10 + (str[i] - '0');
		i++;
	}
	return (number);
}

int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}
