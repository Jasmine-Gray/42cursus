/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_map_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 20:14:08 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 17:49:47 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

static int	check_chars(t_map *map)
{
	int	y;
	int	x;

	y = 0;
	while (map->grid[y])
	{
		x = 0;
		while (map->grid[y][x])
		{
			if (!ft_strchr("01NSEWD ", map->grid[y][x]))
			{
				print_error("Invalid character in map");
				return (1);
			}
			x++;
		}
		y++;
	}
	return (0);
}

static int	check_cell(t_map *map, int y, int x)
{
	int	len;

	if (y == 0 || !map->grid[y + 1])
		return (1);
	if (x == 0)
		return (1);
	len = (int)ft_strlen(map->grid[y - 1]);
	if (x >= len || map->grid[y - 1][x] == ' ')
		return (1);
	len = (int)ft_strlen(map->grid[y + 1]);
	if (x >= len || map->grid[y + 1][x] == ' ')
		return (1);
	if (map->grid[y][x - 1] == ' ')
		return (1);
	if (!map->grid[y][x + 1] || map->grid[y][x + 1] == ' ')
		return (1);
	return (0);
}

static int	check_enclosed(t_map *map)
{
	int	y;
	int	x;

	y = 0;
	while (map->grid[y])
	{
		x = 0;
		while (map->grid[y][x])
		{
			if (map->grid[y][x] == '0' && check_cell(map, y, x))
			{
				print_error("Map is not enclosed by walls");
				return (1);
			}
			x++;
		}
		y++;
	}
	return (0);
}

int	validate_map(t_map *map)
{
	if (check_chars(map))
		return (1);
	if (check_enclosed(map))
		return (1);
	return (0);
}
