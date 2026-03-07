/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 14:47:07 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 22:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

int	close_window(t_data *d)
{
	cleanup_data(d);
	exit(0);
	return (0);
}

void	toggle_door(t_data *d)
{
	int	nx;
	int	ny;

	nx = (int)(d->pos_x + d->dir_x * 0.8);
	ny = (int)(d->pos_y + d->dir_y * 0.8);
	if (ny < 0 || nx < 0 || !d->map[ny] || !d->map[ny][nx])
		return ;
	if (d->map[ny][nx] == 'D')
		d->map[ny][nx] = 'O';
	else if (d->map[ny][nx] == 'O')
		d->map[ny][nx] = 'D';
}

int	mouse_press(int button, int x, int y, t_data *d)
{
	(void)button;
	(void)x;
	(void)y;
	if (!d->mouse_captured)
	{
		d->mouse_captured = 1;
		wrap_mouse_hide(d);
		wrap_mouse_move(d, WIDTH / 2, HEIGHT / 2);
	}
	return (0);
}

int	key_press(int keycode, t_data *d)
{
	if (keycode == KEY_ESC)
		close_window(d);
	if (keycode == KEY_E)
		toggle_door(d);
	if (keycode == KEY_W)
		d->keys.w = 1;
	if (keycode == KEY_A)
		d->keys.a = 1;
	if (keycode == KEY_S)
		d->keys.s = 1;
	if (keycode == KEY_D)
		d->keys.d = 1;
	if (keycode == KEY_LEFT)
		d->keys.left = 1;
	if (keycode == KEY_RIGHT)
		d->keys.right = 1;
	return (0);
}

int	key_release(int keycode, t_data *d)
{
	if (keycode == KEY_W)
		d->keys.w = 0;
	if (keycode == KEY_A)
		d->keys.a = 0;
	if (keycode == KEY_S)
		d->keys.s = 0;
	if (keycode == KEY_D)
		d->keys.d = 0;
	if (keycode == KEY_LEFT)
		d->keys.left = 0;
	if (keycode == KEY_RIGHT)
		d->keys.right = 0;
	return (0);
}
