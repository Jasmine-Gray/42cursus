/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 21:28:37 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 21:28:39 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	try_move(t_data *d, double dx, double dy)
{
	double	nx;
	double	ny;

	nx = d->pos_x + dx;
	ny = d->pos_y + dy;
	if (d->map[(int)d->pos_y][(int)nx] != '1')
		d->pos_x = nx;
	if (d->map[(int)ny][(int)d->pos_x] != '1')
		d->pos_y = ny;
}

static void	move_player(t_data *d)
{
	if (d->keys.w)
		try_move(d, d->dir_x * MOVE_SPEED, d->dir_y * MOVE_SPEED);
	if (d->keys.s)
		try_move(d, -d->dir_x * MOVE_SPEED, -d->dir_y * MOVE_SPEED);
	if (d->keys.d)
		try_move(d, -d->dir_y * MOVE_SPEED, d->dir_x * MOVE_SPEED);
	if (d->keys.a)
		try_move(d, d->dir_y * MOVE_SPEED, -d->dir_x * MOVE_SPEED);
}

static void	rotate_view(t_data *d)
{
	double	a;
	double	old_dx;
	double	old_px;

	a = 0;
	if (d->keys.right)
		a = ROT_SPEED;
	if (d->keys.left)
		a = -ROT_SPEED;
	if (a == 0)
		return ;
	old_dx = d->dir_x;
	d->dir_x = d->dir_x * cos(a) - d->dir_y * sin(a);
	d->dir_y = old_dx * sin(a) + d->dir_y * cos(a);
	old_px = d->plane_x;
	d->plane_x = d->plane_x * cos(a) - d->plane_y * sin(a);
	d->plane_y = old_px * sin(a) + d->plane_y * cos(a);
}

int	game_loop(t_data *d)
{
	move_player(d);
	rotate_view(d);
	render_frame(d);
	mlx_put_image_to_window(d->mlx, d->win, d->img.img, 0, 0);
	return (0);
}
