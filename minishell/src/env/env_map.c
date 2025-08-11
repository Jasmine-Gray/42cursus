/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_map.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 13:56:22 by mishimod          #+#    #+#             */
/*   Updated: 2025/08/09 15:36:38 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*item_new(const char *name, const char *value)
{
	t_env	*new_item;

	new_item = (t_env *)malloc(sizeof(t_env));
	if (!new_item)
		return (NULL);
	new_item->name = ft_strdup(name);
	if (value)
		new_item->value = ft_strdup(value);
	else
		new_item->value = ft_strdup("");
	if (!new_item->name || !new_item->value)
	{
		free(new_item->name);
		free(new_item->value);
		free(new_item);
		return (NULL);
	}
	new_item->next = NULL;
	return (new_item);
}

t_map	*map_new(void)
{
	t_map	*map;

	map = (t_map *)malloc(sizeof(t_map));
	if (!map)
		return (NULL);
	map->head = NULL;
	return (map);
}

int	map_unset(t_map *map, const char *name)
{
	t_env	*current;
	t_env	*prev;

	if (!map || !name)
		return (-1);
	current = map->head;
	prev = NULL;
	while (current)
	{
		if (ft_strncmp(current->name, name, ft_strlen(name)) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				map->head = current->next;
			free(current->name);
			free(current->value);
			free(current);
			return (0);
		}
		prev = current;
		current = current->next;
	}
	return (-1);
}

void	map_free(t_map *map)
{
	t_env	*current;
	t_env	*next;

	if (!map)
		return ;
	current = map->head;
	while (current)
	{
		next = current->next;
		free(current->name);
		free(current->value);
		free(current);
		current = next;
	}
}
