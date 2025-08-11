/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_repl.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 04:53:06 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 21:51:33 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	process_line(char *line, t_ctx *ctx)
{
	char	*trimstr;

	trimstr = line;
	while (*trimstr && (*trimstr == ' ' || *trimstr == '\t'))
		trimstr++;
	if (*trimstr)
	{
		add_history(line);
		interpret(line, ctx);
	}
}

void	run_repl(t_ctx *ctx)
{
	char	*line;

	rl_outstream = stdout;
	while (1)
	{
		if (g_signal_number == SIGINT)
		{
			ctx->last_exit_code = 130;
			g_signal_number = 0;
		}
		line = readline("minishell$ ");
		if (line == NULL)
		{
			ft_dprintf(STDERR_FILENO, "exit\n");
			break ;
		}
		process_line(line, ctx);
		free(line);
	}
	rl_clear_history();
}
