/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 19:15:30 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/28 08:42:09 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

static t_fd_backup	backup_fds(t_redirect *redirects)
{
	t_fd_backup	backup;
	t_redirect	*redir;

	redir = redirects;
	backup.in = -1;
	backup.out = -1;
	backup.err = -1;
	while (redir)
	{
		if (redir->io_operator->flags & WORD_REDIR_IN && backup.in == -1)
			backup.in = dup(STDIN_FILENO);
		else if (redir->io_operator->flags & WORD_HEREDOC && backup.in == -1)
			backup.in = dup(STDIN_FILENO);
		else if ((redir->io_operator->flags & WORD_REDIR_OUT
				|| redir->io_operator->flags & WORD_REDIR_APPEND)
			&& backup.out == -1)
			backup.out = dup(STDOUT_FILENO);
		redir = redir->next;
	}
	return (backup);
}

static void	restore_fds(t_fd_backup *backup)
{
	if (backup->in != -1)
	{
		dup2(backup->in, STDIN_FILENO);
		close(backup->in);
	}
	if (backup->out != -1)
	{
		dup2(backup->out, STDOUT_FILENO);
		close(backup->out);
	}
	if (backup->err != -1)
	{
		dup2(backup->err, STDERR_FILENO);
		close(backup->err);
	}
}

int	execute_builtin(t_node_command *cmd, t_ctx *ctx)
{
	t_fd_backup	backup;
	int			stat;

	backup = backup_fds(cmd->redirects);
	if (do_redirections(cmd->redirects) < 0)
	{
		restore_fds(&backup);
		return (1);
	}
	stat = search_builtin(cmd->args, ctx);
	restore_fds(&backup);
	return (stat);
}
