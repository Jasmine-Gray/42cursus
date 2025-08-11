/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 00:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

int	is_ambiguous_filename(t_word_desc *filename)
{
	size_t	i;

	if (!filename || !filename->word)
		return (1);
	if (filename->flags & (WORD_QUOTED_SINGLE | WORD_QUOTED_DOUBLE))
		return (0);
	if (filename->word[0] == '\0')
		return (1);
	i = 0;
	while (filename->word[i])
	{
		if (is_space(filename->word[i]))
			return (1);
		i++;
	}
	return (0);
}

int	handle_input_redirect(t_redirect *redir)
{
	int	fd;

	fd = open(redir->filename->word, O_RDONLY);
	if (fd < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n",
			redir->filename->word, strerror(errno));
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		close(fd);
		ft_dprintf(STDERR_FILENO, "minishell: dup2: %s\n",
			strerror(errno));
		return (-1);
	}
	close(fd);
	return (0);
}

int	handle_output_redirect(t_redirect *redir, int flags)
{
	int	fd;

	fd = open(redir->filename->word, flags, 0644);
	if (fd < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n",
			redir->filename->word, strerror(errno));
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		close(fd);
		ft_dprintf(STDERR_FILENO, "minishell: dup2: %s\n",
			strerror(errno));
		return (-1);
	}
	close(fd);
	return (0);
}

int	handle_heredoc_redirect(t_redirect *redir)
{
	if (redir->heredoc_fd < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: heredoc error\n");
		return (-1);
	}
	if (dup2(redir->heredoc_fd, STDIN_FILENO) < 0)
	{
		close(redir->heredoc_fd);
		ft_dprintf(STDERR_FILENO, "minishell: dup2: %s\n",
			strerror(errno));
		return (-1);
	}
	close(redir->heredoc_fd);
	return (0);
}
