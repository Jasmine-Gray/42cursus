/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:40:52 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:40:59 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_env(t_word_list *list, t_map *env_map)
{
	t_env	*current;

	if (list && list->next)
	{
		ft_dprintf(STDERR_FILENO, "env: too many arguments\n");
		return (1);
	}
	current = env_map->head;
	while (current)
	{
		if (current->value)
			ft_printf("%s=%s\n", current->name, current->value);
		else
			ft_printf("%s=\n", current->name);
		current = current->next;
	}
	return (0);
}
