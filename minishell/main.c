/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 22:40:48 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/08 17:22:07 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_number = 0;

static void				cleanup_context(t_ctx *ctx);
static void				init_context(t_ctx *ctx, char **envp);

int	main(int argc, char **argv, char **envp)
{
	static t_ctx	ctx = {0};
	int				exit_code;

	(void)argc;
	(void)argv;
	init_context(&ctx, envp);
	setup_signal_handlers();
	run_repl(&ctx);
	exit_code = ctx.last_exit_code;
	cleanup_context(&ctx);
	return (exit_code);
}

static void	init_context(t_ctx *ctx, char **envp)
{
	int		i;
	t_map	*map;

	memory_pool_init(&ctx->memory_pool);
	map = map_new();
	if (!map)
		fatal_error("map_new");
	ctx->env_map = *map;
	free(map);
	ctx->current_cmd = NULL;
	ctx->last_exit_code = 0;
	ctx->heredoc_interrupted = 0;
	i = 0;
	while (envp[i])
	{
		if (map_put(&ctx->env_map, envp[i]) != 0)
			fatal_error("map_put");
		i++;
	}
}

static void	cleanup_context(t_ctx *ctx)
{
	ctx_cleanup_command(ctx);
	memory_pool_cleanup(&ctx->memory_pool);
	map_free(&ctx->env_map);
}
