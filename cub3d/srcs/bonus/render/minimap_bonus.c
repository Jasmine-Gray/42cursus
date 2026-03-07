/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:09:31 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 17:09:31 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

static void	draw_minimap_tile(t_data *d, int mx, int my, int color)
{
	int	px;
	int	py;
	int	sx;
	int	sy;

	sx = MINIMAP_OFFSET + mx * MINIMAP_TILE;
	sy = MINIMAP_OFFSET + my * MINIMAP_TILE;
	py = 0;
	while (py < MINIMAP_TILE)
	{
		px = 0;
		while (px < MINIMAP_TILE)
		{
			my_pixel_put(&d->img, sx + px, sy + py, color);
			px++;
		}
		py++;
	}
}

static void	draw_minimap_row(t_data *d, int y)
{
	int		x;
	char	c;

	x = 0;
	while (d->map[y][x])
	{
		c = d->map[y][x];
		if (c == '1')
			draw_minimap_tile(d, x, y, 0xFFFFFF);
		else if (c == '0' || c == 'N' || c == 'S'
			|| c == 'E' || c == 'W')
			draw_minimap_tile(d, x, y, 0x333333);
		x++;
	}
}

static void	draw_minimap_player(t_data *d)
{
	int	px;
	int	py;
	int	x;
	int	y;

	px = MINIMAP_OFFSET + (int)(d->pos_x * MINIMAP_TILE) - 1;
	py = MINIMAP_OFFSET + (int)(d->pos_y * MINIMAP_TILE) - 1;
	x = 0;
	while (x < 3)
	{
		y = 0;
		while (y < 3)
		{
			my_pixel_put(&d->img, px + x, py + y, 0xFF0000);
			y++;
		}
		x++;
	}
}

static void	draw_minimap_border(t_data *d, int w, int h)
{
	int	i;
	int	ox;
	int	oy;

	ox = MINIMAP_OFFSET - 1;
	oy = MINIMAP_OFFSET - 1;
	i = 0;
	while (i <= w * MINIMAP_TILE + 1)
	{
		my_pixel_put(&d->img, ox + i, oy, 0x888888);
		my_pixel_put(&d->img, ox + i, oy + h * MINIMAP_TILE + 1, 0x888888);
		i++;
	}
	i = 0;
	while (i <= h * MINIMAP_TILE + 1)
	{
		my_pixel_put(&d->img, ox, oy + i, 0x888888);
		my_pixel_put(&d->img, ox + w * MINIMAP_TILE + 1, oy + i, 0x888888);
		i++;
	}
}

void	draw_minimap(t_data *d)
{
	int	y;
	int	max_w;
	int	len;

	y = 0;
	max_w = 0;
	while (d->map[y])
	{
		len = (int)ft_strlen(d->map[y]);
		if (len > max_w)
			max_w = len;
		draw_minimap_row(d, y);
		y++;
	}
	draw_minimap_border(d, max_w, y);
	draw_minimap_player(d);
}
