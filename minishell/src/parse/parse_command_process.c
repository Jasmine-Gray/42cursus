/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command_process.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 09:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/07 09:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"
#include "parse.h"

int	process_tokens(t_ctx *ctx, t_word_list **tokens,
		t_word_list **arg_head, t_ast_node *node)
{
	t_word_list	*arg_tail;
	t_word_list	*current;

	arg_tail = NULL;
	while (*tokens && !((*tokens)->word->flags & WORD_PIPE))
	{
		current = *tokens;
		if (is_redirect_operator(current->word->flags))
		{
			*tokens = (*tokens)->next;
			if (!check_redirect_syntax(tokens))
				return (0);
			add_redirect(&node->as.command.redirects,
				create_redirect(ctx, current->word, (*tokens)->word));
			*tokens = (*tokens)->next;
		}
		else
		{
			if (!add_argument(ctx, arg_head, &arg_tail, current->word))
				return (0);
			*tokens = (*tokens)->next;
		}
	}
	return (1);
}

int	add_argument(t_ctx *ctx, t_word_list **arg_head,
		t_word_list **arg_tail, t_word_desc *word)
{
	t_word_list	*new_arg;

	new_arg = ctx_malloc(ctx, sizeof(t_word_list));
	if (!new_arg)
		return (0);
	new_arg->word = word;
	new_arg->next = NULL;
	if (!*arg_head)
	{
		*arg_head = new_arg;
		*arg_tail = new_arg;
	}
	else
	{
		(*arg_tail)->next = new_arg;
		*arg_tail = new_arg;
	}
	return (1);
}
