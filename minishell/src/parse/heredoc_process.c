/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_process.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 20:01:09 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

int			write_to_pipe(int write_fd, const char *delimiter, t_ctx *ctx);

static int	handle_child_process(int pipefd[2], const char *delimiter,
		t_ctx *ctx)
{
	close(pipefd[0]);
	if (write_to_pipe(pipefd[1], delimiter, ctx) < 0)
	{
		close(pipefd[1]);
		exit(130);
	}
	close(pipefd[1]);
	exit(0);
}

static int	wait_for_heredoc_child(pid_t pid, int read_fd)
{
	int	status;

	while (waitpid(pid, &status, 0) < 0)
		if (errno != EINTR)
			break ;
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		close(read_fd);
		return (-1);
	}
	return (read_fd);
}

static int	create_heredoc_pipe(int pipefd[2])
{
	if (pipe(pipefd) < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: pipe: %s\n", strerror(errno));
		return (-1);
	}
	return (0);
}

int	process_heredoc(const char *delimiter, int expand_vars, t_ctx *ctx)
{
	int		pipefd[2];
	pid_t	pid;

	(void)expand_vars;
	if (create_heredoc_pipe(pipefd) < 0)
		return (-1);
	pid = fork();
	if (pid < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: fork: %s\n", strerror(errno));
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
		handle_child_process(pipefd, delimiter, ctx);
	close(pipefd[1]);
	return (wait_for_heredoc_child(pid, pipefd[0]));
}
