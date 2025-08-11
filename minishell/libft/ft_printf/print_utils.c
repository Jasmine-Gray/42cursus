/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:46:09 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 15:01:26 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

int	len_number(long long num)
{
	int	len;

	len = 1;
	if (num < 0)
	{
		num *= -1;
		len++;
	}
	while (num >= 10)
	{
		num /= 10;
		len++;
	}
	return (len);
}

void	print_nbr(unsigned long long p, int *count)
{
	if (p >= 16)
		print_nbr(p / 16, count);
	ft_putchar_fd(HEX_CONST_LOWER[p % 16], 1);
	(*count)++;
}

void	print_nbr_x(unsigned long long p, int *count, char format)
{
	if (p >= 16)
		print_nbr_x(p / 16, count, format);
	if (format == 'x')
		ft_putchar_fd(HEX_CONST_LOWER[p % 16], 1);
	else
		ft_putchar_fd(HEX_CONST_UPPER[p % 16], 1);
	(*count)++;
}
