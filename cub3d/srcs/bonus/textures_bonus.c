/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   textures_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:45 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 23:48:47 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

static int	load_door_tex(t_data *d)
{
	d->door_tex.img = mlx_xpm_file_to_image(d->mlx, "textures/door.xpm",
			&d->door_tex.width, &d->door_tex.height);
	if (!d->door_tex.img)
		return (print_error("Failed to load door texture"), 1);
	d->door_tex.addr = mlx_get_data_addr(d->door_tex.img,
			&d->door_tex.bits_per_pixel,
			&d->door_tex.line_length,
			&d->door_tex.endian);
	return (0);
}

int	load_textures(t_data *d, t_config *c)
{
	char	*paths[4];
	int		i;

	paths[0] = c->no_texture;
	paths[1] = c->so_texture;
	paths[2] = c->we_texture;
	paths[3] = c->ea_texture;
	i = -1;
	while (++i < 4)
	{
		d->tex[i].img = mlx_xpm_file_to_image(d->mlx, paths[i],
				&d->tex[i].width, &d->tex[i].height);
		if (!d->tex[i].img)
			return (print_error("Failed to load wall texture"), 1);
		d->tex[i].addr = mlx_get_data_addr(d->tex[i].img,
				&d->tex[i].bits_per_pixel,
				&d->tex[i].line_length,
				&d->tex[i].endian);
	}
	return (load_door_tex(d));
}

int	load_sprite_frames(t_data *d)
{
	char	*f_paths[4];
	int		i;

	f_paths[0] = "textures/human_1.xpm";
	f_paths[1] = "textures/human_2.xpm";
	f_paths[2] = "textures/human_3.xpm";
	f_paths[3] = "textures/human_4.xpm";
	d->sprite.frame_count = 4;
	i = -1;
	while (++i < 4)
	{
		d->sprite.frames[i].img = mlx_xpm_file_to_image(d->mlx, f_paths[i],
				&d->sprite.frames[i].width, &d->sprite.frames[i].height);
		if (!d->sprite.frames[i].img)
			return (print_error("Failed to load sprite frame"), 1);
		d->sprite.frames[i].addr = mlx_get_data_addr(d->sprite.frames[i].img,
				&d->sprite.frames[i].bits_per_pixel,
				&d->sprite.frames[i].line_length,
				&d->sprite.frames[i].endian);
	}
	return (0);
}
