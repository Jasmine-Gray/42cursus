/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_loop_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 20:36:45 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 22:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

static void	try_move(t_data *d, double dx, double dy)
{
	double	nx;
	double	ny;

	nx = d->pos_x + dx;
	ny = d->pos_y + dy;
	if (d->map[(int)d->pos_y][(int)nx] != '1'
		&& d->map[(int)d->pos_y][(int)nx] != 'D')
		d->pos_x = nx;
	if (d->map[(int)ny][(int)d->pos_x] != '1'
		&& d->map[(int)ny][(int)d->pos_x] != 'D')
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

static void	apply_rotation(t_data *d, double a)
{
	double	old_dx;
	double	old_px;

	old_dx = d->dir_x;
	d->dir_x = d->dir_x * cos(a) - d->dir_y * sin(a);
	d->dir_y = old_dx * sin(a) + d->dir_y * cos(a);
	old_px = d->plane_x;
	d->plane_x = d->plane_x * cos(a) - d->plane_y * sin(a);
	d->plane_y = old_px * sin(a) + d->plane_y * cos(a);
}

static void	rotate_view(t_data *d)
{
	double	a;
	int		mouse_x;
	int		mouse_y;

	a = 0;
	if (d->keys.right)
		a += ROT_SPEED;
	if (d->keys.left)
		a -= ROT_SPEED;
	if (d->mouse_captured)
	{
		wrap_mouse_get_pos(d, &mouse_x, &mouse_y);
		a += (mouse_x - WIDTH / 2) * MOUSE_SENSITIVITY;
		wrap_mouse_move(d, WIDTH / 2, HEIGHT / 2);
	}
	if (a == 0)
		return ;
	apply_rotation(d, a);
}

int	game_loop(t_data *d)
{
	move_player(d);
	rotate_view(d);
	update_sprite_animation(d);
	render_frame(d);
	mlx_put_image_to_window(d->mlx, d->win, d->img.img, 0, 0);
	return (0);
}
