/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_ast_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 00:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

int	handle_redir_only(t_node_command *cmd)
{
	int	saved_stdin;
	int	saved_stdout;
	int	status;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	status = 0;
	if (do_redirections(cmd->redirects) < 0)
		status = 1;
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	return (status);
}

void	execute_in_child(t_node_command *cmd, t_ctx *ctx)
{
	restore_original_signals();
	if (cmd->redirects && do_redirections(cmd->redirects) < 0)
		exit(1);
	if (is_builtin(cmd->args))
		exit(execute_builtin(cmd, ctx));
	else
		exit(execute_disk_command(cmd->args, ctx));
}

int	wait_for_child(pid_t pid)
{
	int	status;

	while (waitpid(pid, &status, 0) < 0)
	{
		if (errno != EINTR)
		{
			perror("waitpid");
			setup_signals_after_child();
			return (1);
		}
	}
	setup_signals_after_child();
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGQUIT)
		ft_dprintf(STDERR_FILENO, "Quit: %d\n", WTERMSIG(status));
	return (get_exit_status(status));
}
