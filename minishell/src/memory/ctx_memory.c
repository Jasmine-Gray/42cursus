/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_memory.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 09:54:12 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/27 18:35:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ctx_free_word_list(t_ctx *ctx);

void	*ctx_malloc(t_ctx *ctx, size_t size)
{
	void	*ptr;

	ptr = ft_calloc(1, size);
	if (!ptr)
		return (NULL);
	if (memory_pool_add(&ctx->memory_pool, ptr) < 0)
	{
		free(ptr);
		return (NULL);
	}
	return (ptr);
}

char	*ctx_strdup(t_ctx *ctx, const char *s)
{
	char	*dup;

	dup = ft_strdup(s);
	if (!dup)
		return (NULL);
	if (memory_pool_add(&ctx->memory_pool, dup) < 0)
	{
		free(dup);
		return (NULL);
	}
	return (dup);
}

void	ctx_free(t_ctx *ctx, void *ptr)
{
	if (!ptr)
		return ;
	memory_pool_remove(&ctx->memory_pool, ptr);
	free(ptr);
}

void	ctx_cleanup_command(t_ctx *ctx)
{
	if (ctx->current_cmd)
	{
		ctx_free_word_list(ctx);
	}
}

static void	ctx_free_word_list(t_ctx *ctx)
{
	t_word_list	*list;
	t_word_list	*tmp;

	list = ctx->current_cmd;
	while (list)
	{
		tmp = list;
		list = list->next;
		if (tmp->word)
		{
			ctx_free(ctx, tmp->word->word);
			ctx_free(ctx, tmp->word);
		}
		ctx_free(ctx, tmp);
	}
	ctx->current_cmd = NULL;
}
