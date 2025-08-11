/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_flags.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 03:02:42 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/27 08:11:49 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_flags(char *token, char quote)
{
	if (!token || !*token)
		return (WORD_NONE);
	if (token[0] == '>' && token[1] == '>')
		return (WORD_REDIR_APPEND);
	if (token[0] == '<' && token[1] == '<')
		return (WORD_HEREDOC);
	if (quote && quote == '\'')
		return (WORD_QUOTED_SINGLE);
	if (quote && quote == '\"')
		return (WORD_QUOTED_DOUBLE);
	if (token[0] == '|')
		return (WORD_PIPE);
	if (token[0] == '>')
		return (WORD_REDIR_OUT);
	if (token[0] == '<')
		return (WORD_REDIR_IN);
	if (token[0] == '$')
		return (WORD_EXPANDED);
	if (ft_strchr(token, '='))
		return (WORD_ASSIGNMENT);
	return (WORD_NONE);
}
