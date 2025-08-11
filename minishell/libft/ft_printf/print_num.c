/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_num.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:45:38 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/08 15:10:33 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	print_number(int d, int *count)
{
	int	count_num;

	count_num = len_number((long long)d);
	ft_putnbr_fd(d, 1);
	(*count) += count_num;
}

void	print_unsigned_number(unsigned int d, int *count)
{
	char			*str;
	int				count_num;

	str = ft_itoa_unsigned(d);
	count_num = len_number((long long)d);
	if (str)
	{
		ft_putstr_fd(str, STDOUT_FILENO);
		free(str);
	}
	(*count) += count_num;
}

void	print_unsigned_hex_number(unsigned int d, int *count, char format)
{
	print_nbr_x(d, count, format);
}
