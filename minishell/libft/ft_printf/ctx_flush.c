/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_flush.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 18:08:03 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 06:12:39 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	ctx_flush(t_printf_ctx *ctx)
{
	size_t	off;
	ssize_t	w;

	off = 0;
	while (off < ctx->used)
	{
		w = write(ctx->fd, ctx->buffer + off, ctx->used - off);
		if (w < 0)
		{
			if (errno == EINTR)
				continue ;
			ctx->err = -1;
			ctx->used = 0;
			return ;
		}
		off += (size_t)w;
	}
	ctx->used = 0;
}
