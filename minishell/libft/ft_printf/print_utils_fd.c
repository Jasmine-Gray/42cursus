/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils_fd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:46:02 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 15:01:41 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	print_nbr_fd(unsigned long long p, int *count, t_printf_ctx *ctx)
{
	if (p >= 16)
		print_nbr_fd(p / 16, count, ctx);
	ctx_append_char(ctx, HEX_CONST_LOWER[p % 16]);
	(*count)++;
}

void	print_nbr_x_fd(unsigned long long p, int *count, char format,
		t_printf_ctx *ctx)
{
	if (p >= 16)
		print_nbr_x_fd(p / 16, count, format, ctx);
	if (format == 'x')
		ctx_append_char(ctx, HEX_CONST_LOWER[p % 16]);
	else
		ctx_append_char(ctx, HEX_CONST_UPPER[p % 16]);
	(*count)++;
}
