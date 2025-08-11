/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_format.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 15:01:32 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

static void	handle_char_str(char c, va_list *ap, int *n, t_printf_ctx *ctx)
{
	if (c == 'c')
		print_char_fd((char)va_arg(*ap, int), n, ctx);
	else if (c == 's')
		print_string_fd(va_arg(*ap, char *), n, ctx);
}

static void	handle_pointer(va_list *ap, int *n, t_printf_ctx *ctx)
{
	print_pointer_fd(va_arg(*ap, unsigned long long), n, ctx);
}

static void	handle_numbers(char c, va_list *ap, int *n, t_printf_ctx *ctx)
{
	if (c == 'd' || c == 'i')
		print_number_fd(va_arg(*ap, int), n, ctx);
	else if (c == 'u')
		print_unsigned_number_fd(va_arg(*ap, unsigned int), n, ctx);
	else if (c == 'x' || c == 'X')
		print_unsigned_hex_number_fd(va_arg(*ap, unsigned int), n, c, ctx);
}

void	process_format(const char **p, va_list *ap, int *n, t_printf_ctx *ctx)
{
	(*p)++;
	if (**p == 'c' || **p == 's')
		handle_char_str(**p, ap, n, ctx);
	else if (**p == 'p')
		handle_pointer(ap, n, ctx);
	else if (**p == 'd' || **p == 'i' || **p == 'u' || **p == 'x' || **p == 'X')
		handle_numbers(**p, ap, n, ctx);
	else if (**p == '%')
	{
		ctx_append_char(ctx, '%');
		(*n)++;
	}
	(*p)++;
}
