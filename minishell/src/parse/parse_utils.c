/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 09:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/07 09:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

int	is_redirect_operator(int flags)
{
	return (flags & (WORD_REDIR_IN | WORD_REDIR_OUT
			| WORD_REDIR_APPEND | WORD_HEREDOC));
}
