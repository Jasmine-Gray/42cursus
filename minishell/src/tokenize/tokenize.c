/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 01:09:04 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:00:18 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	get_next_token(char **line, t_ctx *ctx, t_token_state *state)
{
	char	*word;
	int		flags;

	while (**line && is_space(**line))
		(*line)++;
	if (!**line)
		return (1);
	flags = 0;
	if (is_operator(**line))
		word = extract_operator(ctx, line, &flags);
	else
		word = extract_concatenated_word(ctx, line, &flags);
	if (!word)
		return (-1);
	add_word_to_list(ctx, state, word, flags);
	return (0);
}

t_word_list	*tokenize(char *line, t_ctx *ctx)
{
	t_word_list		*head;
	t_word_list		*current;
	t_token_state	state;
	int				status;

	head = NULL;
	current = NULL;
	state.head = &head;
	state.current = &current;
	status = 0;
	while (*line && status == 0)
		status = get_next_token(&line, ctx, &state);
	if (status == -1)
		return (NULL);
	return (head);
}
