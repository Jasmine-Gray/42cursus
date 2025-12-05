/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 12:41:39 by mishimod          #+#    #+#             */
/*   Updated: 2025/11/27 21:15:09 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int argc, char **argv)
{
	t_arg	arg;

	if (check_args(argc, argv) < 0)
		return (1);
	assign_args(argc, argv, &arg);
	if (!check_valid_assign(argc, &arg))
	{
		error_phrase();
		return (1);
	}
	init_table(&arg);
	// ft_philo();
	return (0);
}
