/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_word_to_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 19:05:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/27 19:05:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_word_to_list(t_ctx *ctx, t_token_state *state, char *word,
		int flags)
{
	if (!*state->head)
	{
		*state->head = ctx_malloc(ctx, sizeof(t_word_list));
		(*state->head)->word = ctx_malloc(ctx, sizeof(t_word_desc));
		(*state->head)->word->word = word;
		(*state->head)->word->flags = flags;
		(*state->head)->next = NULL;
		*state->current = *state->head;
	}
	else
	{
		(*state->current)->next = ctx_malloc(ctx, sizeof(t_word_list));
		*state->current = (*state->current)->next;
		(*state->current)->word = ctx_malloc(ctx, sizeof(t_word_desc));
		(*state->current)->word->word = word;
		(*state->current)->word->flags = flags;
		(*state->current)->next = NULL;
	}
}
