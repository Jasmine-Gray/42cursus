/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirects.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 08:57:55 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 04:39:43 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

int	is_ambiguous_filename(t_word_desc *filename);
int	handle_input_redirect(t_redirect *redir);
int	handle_output_redirect(t_redirect *redir, int flags);
int	handle_heredoc_redirect(t_redirect *redir);

static int	process_redirect(t_redirect *redir)
{
	if (redir->io_operator->flags & WORD_REDIR_IN)
		return (handle_input_redirect(redir));
	else if (redir->io_operator->flags & WORD_REDIR_OUT)
		return (handle_output_redirect(redir,
				O_WRONLY | O_CREAT | O_TRUNC));
	else if (redir->io_operator->flags & WORD_REDIR_APPEND)
		return (handle_output_redirect(redir,
				O_WRONLY | O_CREAT | O_APPEND));
	else if (redir->io_operator->flags & WORD_HEREDOC)
		return (handle_heredoc_redirect(redir));
	return (0);
}

int	do_redirections(t_redirect *redirects)
{
	t_redirect	*redir;

	redir = redirects;
	while (redir)
	{
		if (is_ambiguous_filename(redir->filename))
		{
			ft_dprintf(STDERR_FILENO, "minishell: ambiguous redirect\n");
			return (-1);
		}
		if (process_redirect(redir) < 0)
			return (-1);
		redir = redir->next;
	}
	return (0);
}
