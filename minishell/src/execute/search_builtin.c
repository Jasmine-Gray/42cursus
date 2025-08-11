/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_builtin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 19:15:30 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/06 18:28:03 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	search_builtin(t_word_list *args, t_ctx *ctx)
{
	int		stat;
	char	*cmd;

	stat = 0;
	if (!args || !args->word || !args->word->word)
		return (-1);
	cmd = args->word->word;
	if (ft_strcmp(cmd, "echo") == 0)
		stat = builtin_echo(args);
	else if (ft_strcmp(cmd, "cd") == 0)
		stat = builtin_cd(args, ctx);
	else if (ft_strcmp(cmd, "pwd") == 0)
		stat = builtin_pwd();
	else if (ft_strcmp(cmd, "export") == 0)
		stat = builtin_export(args, &ctx->env_map);
	else if (ft_strcmp(cmd, "unset") == 0)
		stat = builtin_unset(args, &ctx->env_map);
	else if (ft_strcmp(cmd, "env") == 0)
		stat = builtin_env(args, &ctx->env_map);
	else if (ft_strcmp(cmd, "exit") == 0)
		stat = builtin_exit(args, ctx);
	else
		return (-1);
	return (stat);
}
