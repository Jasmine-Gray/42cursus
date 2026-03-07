/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 16:28:53 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/24 18:30:28 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	count_map_lines(char **lines, int start)
{
	int	i;
	int	last;

	i = 0;
	last = -1;
	while (lines[start + i])
	{
		if (!is_empty_line(lines[start + i]))
			last = i;
		i++;
	}
	return (last + 1);
}

static int	has_empty_line_in_map(char **lines, int start, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		if (is_empty_line(lines[start + i]))
			return (1);
		i++;
	}
	return (0);
}

int	parse_map(char **lines, int start, t_map *map)
{
	int	i;
	int	size;

	size = count_map_lines(lines, start);
	if (size == 0)
		return (print_error("No map found"), 1);
	if (has_empty_line_in_map(lines, start, size))
		return (print_error("Map is not enclosed by walls"), 1);
	map->grid = malloc(sizeof(char *) * (size + 1));
	if (!map->grid)
		return (1);
	i = 0;
	while (i < size)
	{
		map->grid[i] = ft_strdup(lines[start + i]);
		if (!map->grid[i])
			return (1);
		i++;
	}
	map->grid[i] = NULL;
	if (find_player_start(map))
		return (1);
	return (validate_map(map));
}
