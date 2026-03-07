/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_utils_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:44:05 by mishimod          #+#    #+#             */
/*   Updated: 2026/02/28 20:42:50 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

void	my_pixel_put(t_img *img, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
		return ;
	dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

void	draw_bg(t_data *d)
{
	int	x;
	int	y;

	y = 0;
	while (y < HEIGHT)
	{
		x = 0;
		while (x < WIDTH)
		{
			if (y < HEIGHT / 2)
				my_pixel_put(&d->img, x, y, d->ceil_color);
			else
				my_pixel_put(&d->img, x, y, d->floor_color);
			x++;
		}
		y++;
	}
}

int	get_tex_pixel(t_tex *tex, int x, int y)
{
	char	*dst;

	dst = tex->addr + (y * tex->line_length + x * (tex->bits_per_pixel / 8));
	return (*(unsigned int *)dst);
}

void	draw_wall_slice(t_data *d, int x, t_wall *w)
{
	int		tex_x;
	int		tex_y;
	int		y;
	t_tex	*tex;

	if (w->is_door)
		tex = (t_tex *)&d->door_tex;
	else
		tex = &d->tex[w->tex_idx];
	tex_x = (int)(w->wall_x * tex->width);
	if (tex_x >= tex->width)
		tex_x = tex->width - 1;
	y = w->start;
	while (y < w->end)
	{
		tex_y = ((y * 2 - HEIGHT + w->line_h) * tex->height) / (2 * w->line_h);
		if (tex_y >= 0 && tex_y < tex->height)
			my_pixel_put(&d->img, x, y, get_tex_pixel(tex, tex_x, tex_y));
		y++;
	}
}

void	update_sprite_animation(t_data *d)
{
	static int	frame_timer = 0;
	const int	animation_speed = 100;

	frame_timer++;
	if (frame_timer >= animation_speed)
	{
		d->sprite.cur_frame = (d->sprite.cur_frame + 1) % d->sprite.frame_count;
		frame_timer = 0;
	}
}
