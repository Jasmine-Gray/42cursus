/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 21:28:59 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 21:29:01 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	close_window(t_data *d)
{
	cleanup_data(d);
	exit(0);
	return (0);
}

int	key_press(int keycode, t_data *d)
{
	if (keycode == KEY_ESC)
		close_window(d);
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
