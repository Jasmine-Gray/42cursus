/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 09:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/07 09:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

t_redirect	*create_redirect(t_ctx *ctx, t_word_desc *op,
		t_word_desc *filename)
{
	t_redirect	*redir;

	redir = ctx_malloc(ctx, sizeof(t_redirect));
	if (!redir)
		return (NULL);
	redir->io_operator = op;
	redir->filename = filename;
	redir->heredoc_fd = -1;
	redir->next = NULL;
	return (redir);
}

void	add_redirect(t_redirect **head, t_redirect *new_redir)
{
	t_redirect	*current;

	if (!*head)
	{
		*head = new_redir;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}
