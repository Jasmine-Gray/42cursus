/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 08:50:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 21:42:26 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_unset(t_word_list *list, t_map *env_map)
{
	t_word_list	*arg;

	arg = list->next;
	while (arg)
	{
		if (!is_valid_env_identifier(arg->word->word))
			return (print_invalid_identifier("unset", arg->word->word));
		map_unset(env_map, arg->word->word);
		arg = arg->next;
	}
	return (0);
}
