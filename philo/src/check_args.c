/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 19:24:40 by mishimod          #+#    #+#             */
/*   Updated: 2025/09/08 15:04:09 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	is_number(char *str)
{
	int	i;

	i = 0;
	if (!*str)
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	check_args(int argc, char **argv)
{
	int	i;

	(void)argv;
	if (argc != 5 && argc != 6)
	{
		error_phrase();
		return (-1);
	}
	i = 1;
	while (i < argc)
	{
		if (!is_number(argv[i]))
		{
			error_phrase();
			return (-1);
		}
		i++;
	}
	return (0);
}
