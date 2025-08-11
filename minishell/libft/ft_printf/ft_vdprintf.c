/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_vdprintf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 20:58:38 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 07:15:27 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

static void	process_plain_text(const char **p, int *n, t_printf_ctx *ctx)
{
	size_t	len;

	ctx->perc = ft_strchr(*p, '%');
	if (ctx->perc)
		len = (size_t)(ctx->perc - *p);
	else
		len = ft_strlen(*p);
	ctx_append_mem(ctx, *p, len);
	*n += (int)len;
	*p += len;
}

static void	init_ctx(t_printf_ctx *ctx, int fd)
{
	ctx->fd = fd;
	ctx->used = 0;
	ctx->err = 0;
	ctx->perc = NULL;
}

static char	next_conv(const char **p)
{
	char	c;

	(*p)++;
	c = **p;
	(*p)++;
	return (c);
}

static void	process_conversion(const char *format, int *n, t_printf_ctx *ctx,
		va_list ap)
{
	const char	*p;
	char		conv;

	p = format;
	while (*p != '\0')
	{
		if (*p == '%')
		{
			conv = next_conv(&p);
			if (conv == 's')
				print_string_fd(va_arg(ap, char *), n, ctx);
			else if (conv == 'c')
				print_char_fd(va_arg(ap, int), n, ctx);
			else if (conv == 'd' || conv == 'i')
				print_number_fd(va_arg(ap, int), n, ctx);
		}
		else
			process_plain_text(&p, n, ctx);
	}
}

int	ft_vdprintf(int fd, const char *format, va_list ap)
{
	int				n;
	t_printf_ctx	ctx;

	n = 0;
	if (format == NULL)
		return (-1);
	init_ctx(&ctx, fd);
	process_conversion(format, &n, &ctx, ap);
	ctx_flush(&ctx);
	return (n);
}
