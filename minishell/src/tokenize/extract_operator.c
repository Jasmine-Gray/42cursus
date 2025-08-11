/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_operator.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 19:01:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/27 19:01:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*handle_pipe(t_ctx *ctx, char **line, int *flags)
{
	char	*result;

	*flags = WORD_PIPE;
	result = ctx_strdup(ctx, "|");
	(*line)++;
	return (result);
}

static char	*handle_redirect_in(t_ctx *ctx, char **line, int *flags)
{
	char	*result;

	if (*(*line + 1) == '<')
	{
		*flags = WORD_HEREDOC;
		result = ctx_strdup(ctx, "<<");
		(*line) += 2;
	}
	else
	{
		*flags = WORD_REDIR_IN;
		result = ctx_strdup(ctx, "<");
		(*line)++;
	}
	return (result);
}

static char	*handle_redirect_out(t_ctx *ctx, char **line, int *flags)
{
	char	*result;

	if (*(*line + 1) == '>')
	{
		*flags = WORD_REDIR_APPEND;
		result = ctx_strdup(ctx, ">>");
		(*line) += 2;
	}
	else
	{
		*flags = WORD_REDIR_OUT;
		result = ctx_strdup(ctx, ">");
		(*line)++;
	}
	return (result);
}

char	*extract_operator(t_ctx *ctx, char **line, int *flags)
{
	if (**line == '|')
		return (handle_pipe(ctx, line, flags));
	else if (**line == '<')
		return (handle_redirect_in(ctx, line, flags));
	else if (**line == '>')
		return (handle_redirect_out(ctx, line, flags));
	else
	{
		*flags = WORD_NONE;
		return (ctx_strdup(ctx, ""));
	}
}
