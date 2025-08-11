/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 12:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:17:39 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"
#include "parse.h"

static int	process_pipe(t_ctx *ctx, t_word_list **tokens, t_ast_node **left)
{
	t_ast_node	*right;

	*tokens = (*tokens)->next;
	if (!*tokens)
	{
		ft_dprintf(STDERR_FILENO, "syntax error near unexpected token `|'\n");
		return (0);
	}
	if ((*tokens)->word->flags & WORD_PIPE)
	{
		ft_dprintf(STDERR_FILENO, "syntax error near unexpected token `|'\n");
		return (0);
	}
	right = parse_command(ctx, tokens);
	if (!right)
		return (0);
	*left = create_pipe_node(ctx, *left, right);
	if (!*left)
		return (0);
	return (1);
}

t_ast_node	*parse(t_ctx *ctx, t_word_list *tokens)
{
	t_ast_node	*left;

	if (!tokens)
		return (NULL);
	left = parse_command(ctx, &tokens);
	if (!left)
		return (NULL);
	while (tokens && (tokens->word->flags & WORD_PIPE))
	{
		if (!process_pipe(ctx, &tokens, &left))
			return (NULL);
	}
	return (left);
}
