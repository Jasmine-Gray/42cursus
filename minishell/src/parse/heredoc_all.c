/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_all.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 00:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

int	process_heredoc(const char *delimiter, int expand_vars, t_ctx *ctx);

static int	process_command_heredocs(t_ast_node *node, t_ctx *ctx)
{
	t_redirect	*redir;
	int			is_quoted;
	char		*delimiter;

	redir = node->as.command.redirects;
	while (redir)
	{
		if (redir->io_operator->flags & WORD_HEREDOC)
		{
			is_quoted = (redir->filename->flags
					& (WORD_QUOTED_SINGLE | WORD_QUOTED_DOUBLE)) != 0;
			delimiter = redir->filename->word;
			redir->heredoc_fd = process_heredoc(delimiter,
					!is_quoted, ctx);
			if (redir->heredoc_fd < 0)
				return (-1);
		}
		redir = redir->next;
	}
	return (0);
}

int	process_all_heredocs(t_ast_node *node, t_ctx *ctx)
{
	(void)ctx;
	if (!node)
		return (0);
	if (node->type == NODE_COMMAND)
		return (process_command_heredocs(node, ctx));
	else if (node->type == NODE_PIPE)
	{
		if (process_all_heredocs(node->as.pipe.left, ctx) < 0)
			return (-1);
		if (process_all_heredocs(node->as.pipe.right, ctx) < 0)
			return (-1);
	}
	return (0);
}
