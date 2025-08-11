/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_str.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:45:54 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/08 15:10:54 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	print_char(char c, int *count)
{
	ft_putchar_fd(c, 1);
	(*count)++;
}

void	print_string(char *s, int *count)
{
	size_t	str_count;

	str_count = 0;
	if (!s)
	{
		ft_putstr_fd("(null)", STDOUT_FILENO);
		(*count) += 6;
	}
	else
	{
		str_count = ft_strlen(s);
		ft_putstr_fd(s, STDOUT_FILENO);
	}
	(*count) += str_count;
}

void	print_pointer(unsigned long long p, int *count)
{
	write(1, "0x", 2);
	(*count) += 2;
	print_nbr(p, count);
}
