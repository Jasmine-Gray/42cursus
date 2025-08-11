/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/30 16:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_variable_name(const char *str, size_t *len);
char	*get_variable_value(const char *name, size_t len, t_ctx *ctx);

static char	*append_char_to_result(char *result, char c)
{
	char	*temp;

	temp = malloc(ft_strlen(result) + 2);
	if (!temp)
	{
		free(result);
		return (NULL);
	}
	ft_strlcpy(temp, result, ft_strlen(result) + 1);
	temp[ft_strlen(result)] = c;
	temp[ft_strlen(result) + 1] = '\0';
	free(result);
	return (temp);
}

static char	*append_variable_to_result(char *result, const char *str,
	size_t *i, t_ctx *ctx)
{
	size_t	var_len;
	char	*var_value;
	char	*temp;

	(*i)++;
	get_variable_name(&str[*i], &var_len);
	var_value = get_variable_value(&str[*i], var_len, ctx);
	if (var_value)
	{
		temp = ft_strjoin(result, var_value);
		free(result);
		free(var_value);
		result = temp;
		if (!result)
			return (NULL);
	}
	*i += var_len;
	return (result);
}

char	*expand_variable(const char *str, t_ctx *ctx)
{
	char	*result;
	size_t	i;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] && (ft_isalnum(str[i + 1])
				|| str[i + 1] == '_' || str[i + 1] == '?'))
		{
			result = append_variable_to_result(result, str, &i, ctx);
			if (!result)
				return (NULL);
		}
		else
		{
			result = append_char_to_result(result, str[i]);
			if (!result)
				return (NULL);
			i++;
		}
	}
	return (result);
}

void	expand_word_list(t_word_list *list, t_ctx *ctx)
{
	t_word_list	*current;
	char		*expanded;

	current = list;
	while (current)
	{
		if ((current->word->flags & WORD_EXPANDED)
			&& !(current->word->flags & WORD_QUOTED_SINGLE))
		{
			expanded = expand_variable(current->word->word, ctx);
			if (expanded)
			{
				ctx_free(ctx, current->word->word);
				current->word->word = ctx_strdup(ctx, expanded);
				free(expanded);
				current->word->flags &= ~WORD_EXPANDED;
			}
		}
		current = current->next;
	}
}
