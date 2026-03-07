/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map_utils_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 19:35:31 by mishimod          #+#    #+#             */
/*   Updated: 2026/02/27 11:50:41 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

void	set_player_pos(t_map *map, int x, int y, char dir)
{
	map->player_x = (double)x + 0.5;
	map->player_y = (double)y + 0.5;
	map->player_dir = dir;
}

int	find_player_start(t_map *map)
{
	int	y;
	int	x;
	int	found;

	y = 0;
	found = 0;
	while (map->grid[y])
	{
		x = 0;
		while (map->grid[y][x])
		{
			if (ft_strchr("NSEW", map->grid[y][x]))
			{
				if (found++)
					return (print_error("Multiple player starts"), 1);
				set_player_pos(map, x, y, map->grid[y][x]);
				map->grid[y][x] = '0';
			}
			x++;
		}
		y++;
	}
	if (!found)
		return (print_error("No player start found"), 1);
	return (0);
}
