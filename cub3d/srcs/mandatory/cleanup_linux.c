/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_linux.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 00:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	cleanup_data(t_data *d)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		if (d->tex[i].img)
			mlx_destroy_image(d->mlx, d->tex[i].img);
		i++;
	}
	if (d->img.img)
		mlx_destroy_image(d->mlx, d->img.img);
	if (d->win)
		mlx_destroy_window(d->mlx, d->win);
	if (d->mlx)
	{
		mlx_destroy_display(d->mlx);
		free(d->mlx);
	}
	free_split(d->map);
}
