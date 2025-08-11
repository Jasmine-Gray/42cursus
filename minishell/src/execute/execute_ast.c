/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_ast.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 12:10:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 20:03:46 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

static int	error_message(void)
{
	ft_dprintf(STDERR_FILENO, "minishell: : command not found\n");
	return (127);
}

static int	execute_command(t_node_command *cmd, t_ctx *ctx,
		t_exec_ctx exec_ctx)
{
	pid_t	pid;

	if (!cmd->args && cmd->redirects)
		return (handle_redir_only(cmd));
	if (!cmd->args)
		return (0);
	if (!cmd->args->word || !cmd->args->word->word)
		return (0);
	if (cmd->args->word->word[0] == '\0')
		return (error_message());
	if (is_builtin(cmd->args) && exec_ctx == EXEC_PARENT)
		return (execute_builtin(cmd, ctx));
	ignore_signals_in_parent();
	pid = fork();
	if (pid < 0)
	{
		ft_dprintf(STDERR_FILENO, "minishell: fork: %s\n", strerror(errno));
		setup_signals_after_child();
		return (1);
	}
	if (pid == 0)
		execute_in_child(cmd, ctx);
	return (wait_for_child(pid));
}

int	execute_ast(t_ast_node *node, t_ctx *ctx, t_exec_ctx exec_ctx)
{
	if (!node)
		return (0);
	if (node->type == NODE_COMMAND)
		return (execute_command(&node->as.command, ctx, exec_ctx));
	else if (node->type == NODE_PIPE)
		return (execute_pipeline(node->as.pipe.left, node->as.pipe.right, ctx));
	return (0);
}
