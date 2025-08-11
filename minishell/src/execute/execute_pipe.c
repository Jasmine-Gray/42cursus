/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 00:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

void	exec_right_child(int pipefd[2], t_ast_node *right, t_ctx *ctx);
pid_t	fork_right(int pipefd[2], t_ast_node *right, t_ctx *ctx,
			pid_t pid_left);

static void	exec_left_child(int pipefd[2], t_ast_node *left, t_ctx *ctx)
{
	restore_original_signals();
	close(pipefd[0]);
	if (dup2(pipefd[1], STDOUT_FILENO) < 0)
		exit(1);
	close(pipefd[1]);
	exit(execute_ast(left, ctx, EXEC_CHILD));
}

static int	wait_for_both(pid_t pid_left, pid_t pid_right)
{
	int	status_left;
	int	status_right;

	while (waitpid(pid_left, &status_left, 0) < 0)
		if (errno != EINTR)
			break ;
	while (waitpid(pid_right, &status_right, 0) < 0)
		if (errno != EINTR)
			break ;
	if (WIFSIGNALED(status_right) && WTERMSIG(status_right) == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	else if (WIFSIGNALED(status_right) && WTERMSIG(status_right) == SIGQUIT)
		ft_dprintf(STDERR_FILENO, "Quit: %d\n", WTERMSIG(status_right));
	return (get_exit_status(status_right));
}

static int	fork_children(int pipefd[2], t_ast_node *left, t_ast_node *right,
	t_ctx *ctx)
{
	pid_t	pid_left;
	pid_t	pid_right;

	pid_left = fork();
	if (pid_left < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: fork: %s\n", strerror(errno));
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid_left == 0)
		exec_left_child(pipefd, left, ctx);
	pid_right = fork_right(pipefd, right, ctx, pid_left);
	if (pid_right < 0)
		return (-1);
	close(pipefd[0]);
	close(pipefd[1]);
	return (wait_for_both(pid_left, pid_right));
}

int	execute_pipeline(t_ast_node *left, t_ast_node *right, t_ctx *ctx)
{
	int	pipefd[2];
	int	result;

	map_set(&ctx->env_map, "MINISHELL_SUBSHELL", "1");
	if (pipe(pipefd) < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: pipe: %s\n", strerror(errno));
		map_unset(&ctx->env_map, "MINISHELL_SUBSHELL");
		return (1);
	}
	ignore_signals_in_parent();
	result = fork_children(pipefd, left, right, ctx);
	setup_signals_after_child();
	map_unset(&ctx->env_map, "MINISHELL_SUBSHELL");
	if (result < 0)
		return (1);
	return (result);
}
