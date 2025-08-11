/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command.c                                    :+:      :+:    :+:   */
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

t_ast_node	*parse_command(t_ctx *ctx, t_word_list **tokens)
{
	t_ast_node	*node;
	t_word_list	*arg_head;

	node = create_command_node(ctx);
	if (!node)
		return (NULL);
	arg_head = NULL;
	if (!process_tokens(ctx, tokens, &arg_head, node))
		return (NULL);
	node->as.command.args = arg_head;
	return (node);
}
