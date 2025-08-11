/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_unquoted_part.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 19:03:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/27 19:03:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_unquoted_part(t_ctx *ctx, char **line)
{
	char	*start;
	char	*part;
	size_t	len;

	start = *line;
	while (**line && !is_space(**line) && !is_operator(**line)
		&& !get_quote(**line))
		(*line)++;
	len = *line - start;
	part = ctx_malloc(ctx, len + 1);
	ft_strlcpy(part, start, len + 1);
	return (part);
}
