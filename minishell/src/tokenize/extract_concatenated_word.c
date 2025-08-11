/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_concatenated_word.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 19:04:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 04:17:13 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	set_final_word_flags(int *flags, const char *result, int has_sq,
		int has_ud)
{
	*flags = WORD_NONE;
	if (has_sq && !has_ud)
		*flags |= WORD_QUOTED_SINGLE;
	if (ft_strchr(result, '$'))
		*flags |= WORD_EXPANDED;
}

static char	*extract_next_part(t_ctx *ctx, char **line, int *p_has_sq,
		int *p_has_ud)
{
	char	*temp;
	char	quote;
	int		dummy_flags;

	quote = get_quote(**line);
	if (quote)
	{
		temp = extract_quoted_part(ctx, line, &dummy_flags, quote);
		if (quote == '\'')
			*p_has_sq = 1;
	}
	else
	{
		temp = extract_unquoted_part(ctx, line);
		if (temp && ft_strchr(temp, '$'))
			*p_has_ud = 1;
	}
	return (temp);
}

static char	*perform_extraction_loop(t_ctx *ctx, char **line, int *p_has_sq,
		int *p_has_ud)
{
	char	*result;
	char	*temp;

	result = ctx_strdup(ctx, "");
	while (result && **line && !is_space(**line) && !is_operator(**line))
	{
		temp = extract_next_part(ctx, line, p_has_sq, p_has_ud);
		if (!temp)
		{
			ctx_free(ctx, result);
			return (NULL);
		}
		result = ctx_strjoin(ctx, result, temp);
		ctx_free(ctx, temp);
	}
	return (result);
}

char	*extract_concatenated_word(t_ctx *ctx, char **line, int *flags)
{
	char	*result;
	int		has_single_quote;
	int		has_unquoted_dollar;

	has_single_quote = 0;
	has_unquoted_dollar = 0;
	result = perform_extraction_loop(ctx, line, &has_single_quote,
			&has_unquoted_dollar);
	if (!result)
		return (NULL);
	set_final_word_flags(flags, result, has_single_quote, has_unquoted_dollar);
	return (result);
}
