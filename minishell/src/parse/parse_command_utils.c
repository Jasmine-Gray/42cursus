/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 09:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:19:35 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"
#include "parse.h"

int	check_redirect_syntax(t_word_list **tokens)
{
	if (!*tokens)
	{
		ft_dprintf(STDERR_FILENO,
			"minishell: syntax error near unexpected token `newline'\n");
		return (0);
	}
	if ((*tokens)->word->flags & WORD_PIPE)
	{
		ft_dprintf(STDERR_FILENO,
			"minishell: syntax error near unexpected token `|'\n");
		return (0);
	}
	if (is_redirect_operator((*tokens)->word->flags))
	{
		ft_dprintf(STDERR_FILENO,
			"minishell: syntax error near unexpected token `%s'\n",
			(*tokens)->word->word);
		return (0);
	}
	return (1);
}
