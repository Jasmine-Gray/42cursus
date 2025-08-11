/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_set.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 08:27:26 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 08:27:36 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	map_set(t_map *map, const char *name, const char *value)
{
	t_env	*current;
	t_env	*new_node;

	current = map->head;
	while (current)
	{
		if (ft_strcmp(current->name, name) == 0)
		{
			free(current->value);
			if (value)
				current->value = ft_strdup(value);
			else
				current->value = ft_strdup("");
			if (!current->value)
				return (-1);
			return (0);
		}
		current = current->next;
	}
	new_node = item_new(name, value);
	if (!new_node)
		return (-1);
	new_node->next = map->head;
	map->head = new_node;
	return (0);
}
