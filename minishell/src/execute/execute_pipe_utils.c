/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/10 00:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

void	exec_right_child(int pipefd[2], t_ast_node *right, t_ctx *ctx)
{
	restore_original_signals();
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) < 0)
		exit(1);
	close(pipefd[0]);
	exit(execute_ast(right, ctx, EXEC_CHILD));
}

pid_t	fork_right(int pipefd[2], t_ast_node *right, t_ctx *ctx,
	pid_t pid_left)
{
	pid_t	pid_right;
	int		status;

	pid_right = fork();
	if (pid_right < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: fork: %s\n", strerror(errno));
		kill(pid_left, SIGKILL);
		waitpid(pid_left, &status, 0);
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid_right == 0)
		exec_right_child(pipefd, right, ctx);
	return (pid_right);
}
