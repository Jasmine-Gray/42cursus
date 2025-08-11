/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   identifier.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 22:05:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 21:42:20 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_start_char(char c)
{
	return (ft_isalpha(c) || c == '_');
}

static int	is_valid_body_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

int	is_valid_env_identifier(const char *s)
{
	size_t	i;

	if (s == NULL || s[0] == '\0')
		return (0);
	if (!is_valid_start_char(s[0]))
		return (0);
	i = 1;
	while (s[i] != '\0')
	{
		if (!is_valid_body_char(s[i]))
			return (0);
		i++;
	}
	return (1);
}

int	print_invalid_identifier(const char *cmd, const char *word)
{
	ft_dprintf(STDERR_FILENO, "minishell: %s: `%s': not a valid identifier\n",
		cmd, word);
	return (1);
}
