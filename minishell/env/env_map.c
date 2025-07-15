/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_map.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 20:29:05 by mishimod          #+#    #+#             */
/*   Updated: 2025/07/15 20:29:08 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// 新しいitemを作成
t_env	*item_new(const char *name, const char *value)
{
	t_env	*new_item = (t_env *)malloc(sizeof(t_env));
	if (!new_item) return (NULL);
	new_item->name = strdup(name);
	new_item->value = value ? strdup(value) : strdup("");
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

// 新しいmapを作成
t_map	*map_new(void)
{
	t_map	*map = (t_map *)malloc(sizeof(t_map));
	if (!map) return (NULL);
	map->head = NULL;
	return (map);
}

// mapにキーと値のペアを設定 (存在すれば上書き、なければ新規作成)
int	map_set(t_map *map, const char *name, const char *value)
{
	t_env *current = map->head;
	while (current)
	{
		if (strcmp(current->name, name) == 0)
		{
			free(current->value);
			current->value = value ? strdup(value) : strdup("");
			return (current->value ? 0 : -1);
		}
		current = current->next;
	}
	t_env *new_node = item_new(name, value);
	if (!new_node) return (-1);
	new_node->next = map->head;
	map->head = new_node;
	return (0);
}

// "KEY=VALUE"形式の文字列からmapに要素を追加
int	map_put(t_map *map, const char *string)
{
	char *name, *value;
	char *equal_ptr = strchr(string, '=');
	if (!equal_ptr)
		return (map_set(map, string, NULL));

	name = strndup(string, equal_ptr - string);
	if (!name) return (-1);
	value = equal_ptr + 1;
	int result = map_set(map, name, value);
	free(name);
	return (result);
}

// map全体を解放
void	map_free(t_map *map)
{
	t_env *current, *next;
	if (!map) return;
	current = map->head;
	while (current)
	{
		next = current->next;
		free(current->name);
		free(current->value);
		free(current);
		current = next;
	}
	free(map);
}

