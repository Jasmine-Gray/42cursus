/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_quoted_part.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 19:02:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/10 18:38:58 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_quoted_part(t_ctx *ctx, char **line, int *flags, char quote)
{
	char	*start;
	char	*part;
	size_t	len;

	(*line)++;
	start = *line;
	while (**line && **line != quote)
		(*line)++;
	if (!**line)
	{
		ft_dprintf(STDERR_FILENO,
			"minishell: unexpected EOF while looking for matching `%c'\n",
			quote);
		return (NULL);
	}
	len = *line - start;
	part = ctx_malloc(ctx, len + 1);
	ft_strlcpy(part, start, len + 1);
	(*line)++;
	if (quote == '\'')
		*flags |= WORD_QUOTED_SINGLE;
	else if (quote == '"')
		*flags |= WORD_QUOTED_DOUBLE;
	return (part);
}
