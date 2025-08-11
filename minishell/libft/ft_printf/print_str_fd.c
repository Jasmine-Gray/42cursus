/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_str_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:45:47 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/10 17:43:21 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	print_char_fd(char c, int *count, t_printf_ctx *ctx)
{
	ctx_append_char(ctx, c);
	(*count)++;
}

void	print_string_fd(char *s, int *count, t_printf_ctx *ctx)
{
	size_t	str_count;
	int		null_len;

	null_len = 6;
	str_count = 0;
	if (!s)
	{
		ctx_append_mem(ctx, "(null)", null_len);
		(*count) += null_len;
	}
	else
	{
		str_count = ft_strlen(s);
		ctx_append_mem(ctx, s, str_count);
	}
	(*count) += str_count;
}

void	print_pointer_fd(unsigned long long p, int *count, t_printf_ctx *ctx)
{
	ctx_append_mem(ctx, "0x", 2);
	(*count) += 2;
	print_nbr_fd(p, count, ctx);
}
