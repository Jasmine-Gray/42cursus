/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 14:09:39 by mishimod          #+#    #+#             */
/*   Updated: 2025/08/11 21:43:42 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	set_variable(t_map *env_map, char *word)
{
	char	*equal;
	int		ret;
	char	*key;

	equal = ft_strchr(word, '=');
	if (!equal)
		return (0);
	ret = 0;
	{
		key = ft_strndup(word, equal - word);
		if (!key)
			return (1);
		if (!is_valid_env_identifier(key))
		{
			free(key);
			return (print_invalid_identifier("export", word));
		}
		ret = map_set(env_map, key, equal + 1);
		free(key);
	}
	return (ret != 0);
}

int	builtin_export(t_word_list *list, t_map *env_map)
{
	t_word_list	*arg;

	arg = list->next;
	if (!arg)
	{
		builtin_env(list, env_map);
		return (0);
	}
	while (arg)
	{
		if (set_variable(env_map, arg->word->word))
			return (1);
		arg = arg->next;
	}
	return (0);
}
