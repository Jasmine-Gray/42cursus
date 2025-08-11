/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 00:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	g_signal_number = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
}

static void	setup_heredoc_signal(struct sigaction *sa, struct sigaction *old)
{
	ft_memset(sa, 0, sizeof(*sa));
	sa->sa_handler = heredoc_sigint_handler;
	sigemptyset(&sa->sa_mask);
	sa->sa_flags = 0;
	sigaction(SIGINT, sa, old);
}

static int	read_heredoc_line(int fd, const char *delimiter, char **line)
{
	*line = readline("> ");
	if (!*line)
	{
		ft_dprintf(STDERR_FILENO,
			"minishell: warning: here-document delimited by "
			"end-of-file (wanted `%s')\n", delimiter);
		return (1);
	}
	if (ft_strcmp(*line, delimiter) == 0)
	{
		free(*line);
		return (1);
	}
	write(fd, *line, ft_strlen(*line));
	write(fd, "\n", 1);
	free(*line);
	return (0);
}

int	write_to_pipe(int write_fd, const char *delimiter, t_ctx *ctx)
{
	char				*line;
	struct sigaction	sa;
	struct sigaction	old_sa;

	ctx->heredoc_interrupted = 0;
	g_signal_number = 0;
	setup_heredoc_signal(&sa, &old_sa);
	while (g_signal_number != SIGINT)
	{
		if (read_heredoc_line(write_fd, delimiter, &line))
			break ;
	}
	sigaction(SIGINT, &old_sa, NULL);
	if (g_signal_number == SIGINT)
	{
		ctx->heredoc_interrupted = 1;
		g_signal_number = 0;
		return (-1);
	}
	return (0);
}
