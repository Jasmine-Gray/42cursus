/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse_macos_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:09:31 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 17:09:31 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

void	wrap_mouse_get_pos(t_data *d, int *x, int *y)
{
	mlx_mouse_get_pos(d->win, x, y);
}

void	wrap_mouse_move(t_data *d, int x, int y)
{
	mlx_mouse_move(d->win, x, y);
}

void	wrap_mouse_hide(t_data *d)
{
	(void)d;
	mlx_mouse_hide();
}

void	wrap_mouse_show(t_data *d)
{
	(void)d;
	mlx_mouse_show();
}
