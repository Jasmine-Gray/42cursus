/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_append_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 18:19:48 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 02:06:47 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

static void	write_large_data(t_printf_ctx *ctx, const void *data, size_t len)
{
	const char	*p;
	size_t		off;
	ssize_t		w;

	ctx_flush(ctx);
	p = (const char *)data;
	off = 0;
	while (off < len)
	{
		w = write(ctx->fd, p + off, len - off);
		if (w < 0)
		{
			if (errno == EINTR)
				continue ;
			ctx->err = -1;
			return ;
		}
		off += (size_t)w;
	}
}

void	ctx_append_mem(t_printf_ctx *ctx, const void *data, size_t len)
{
	if (len == 0)
		return ;
	if (len >= BUFFER_SIZE)
	{
		write_large_data(ctx, data, len);
		return ;
	}
	if (ctx->used + len > BUFFER_SIZE)
		ctx_flush(ctx);
	ft_memcpy(ctx->buffer + ctx->used, data, len);
	ctx->used += len;
}
