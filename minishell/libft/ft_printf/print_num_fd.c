/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_num_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:45:31 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/08 17:09:34 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	print_number_fd(int d, int *count, t_printf_ctx *ctx)
{
	char	*str;
	int		count_num;

	str = ft_itoa(d);
	if (str)
	{
		count_num = ft_strlen(str);
		ctx_append_mem(ctx, str, count_num);
		(*count) += count_num;
		free(str);
	}
}

void	print_unsigned_number_fd(unsigned int d, int *count, t_printf_ctx *ctx)
{
	char			*str;
	int				count_num;

	str = ft_itoa_unsigned(d);
	if (str)
	{
		count_num = ft_strlen(str);
		ctx_append_mem(ctx, str, count_num);
		(*count) += count_num;
		free(str);
	}
}

void	print_unsigned_hex_number_fd(unsigned int d, int *count, char format,
		t_printf_ctx *ctx)
{
	print_nbr_x_fd(d, count, format, ctx);
}
