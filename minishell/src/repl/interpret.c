/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interpret.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 17:37:25 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/10 18:41:08 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"
#include "parse.h"

static int	handle_tokenize_error(t_ctx *ctx)
{
	ctx_cleanup_command(ctx);
	memory_pool_cleanup(&ctx->memory_pool);
	memory_pool_init(&ctx->memory_pool);
	ctx->last_exit_code = 2;
	return (2);
}

static int	handle_parse_error(t_ctx *ctx)
{
	ctx_cleanup_command(ctx);
	memory_pool_cleanup(&ctx->memory_pool);
	memory_pool_init(&ctx->memory_pool);
	ctx->last_exit_code = 2;
	return (2);
}

int	interpret(char *line, t_ctx *ctx)
{
	t_ast_node	*ast;
	int			wstatus;

	ctx_cleanup_command(ctx);
	ctx->current_cmd = tokenize(line, ctx);
	if (!ctx->current_cmd)
		return (handle_tokenize_error(ctx));
	expand_word_list(ctx->current_cmd, ctx);
	ast = parse(ctx, ctx->current_cmd);
	if (!ast)
		return (handle_parse_error(ctx));
	if (process_all_heredocs(ast, ctx) < 0)
	{
		ctx->last_exit_code = 130;
		ctx_cleanup_command(ctx);
		memory_pool_cleanup(&ctx->memory_pool);
		memory_pool_init(&ctx->memory_pool);
		return (130);
	}
	wstatus = execute_ast(ast, ctx, EXEC_PARENT);
	ctx->last_exit_code = wstatus;
	ctx_cleanup_command(ctx);
	memory_pool_cleanup(&ctx->memory_pool);
	memory_pool_init(&ctx->memory_pool);
	return (wstatus);
}
