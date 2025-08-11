/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_node.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 09:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/07 09:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

t_ast_node	*create_command_node(t_ctx *ctx)
{
	t_ast_node	*node;

	node = ctx_malloc(ctx, sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = NODE_COMMAND;
	node->as.command.args = NULL;
	node->as.command.redirects = NULL;
	return (node);
}

t_ast_node	*create_pipe_node(t_ctx *ctx, t_ast_node *left,
		t_ast_node *right)
{
	t_ast_node	*node;

	node = ctx_malloc(ctx, sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = NODE_PIPE;
	node->as.pipe.left = left;
	node->as.pipe.right = right;
	return (node);
}
