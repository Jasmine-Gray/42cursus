/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:40:21 by mishimod          #+#    #+#             */
/*   Updated: 2026/02/28 22:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

static void	init_ray(t_data *d, double r_dx, double r_dy, t_ray *ray)
{
	ray->dir[0] = r_dx;
	ray->dir[1] = r_dy;
	ray->map[0] = (int)d->pos_x;
	ray->map[1] = (int)d->pos_y;
	ray->delta[0] = fabs(1 / r_dx);
	ray->delta[1] = fabs(1 / r_dy);
	ray->step[0] = 1;
	ray->side_dist[0] = (ray->map[0] + 1.0 - d->pos_x) * ray->delta[0];
	if (r_dx < 0)
	{
		ray->step[0] = -1;
		ray->side_dist[0] = (d->pos_x - ray->map[0]) * ray->delta[0];
	}
	ray->step[1] = 1;
	ray->side_dist[1] = (ray->map[1] + 1.0 - d->pos_y) * ray->delta[1];
	if (r_dy < 0)
	{
		ray->step[1] = -1;
		ray->side_dist[1] = (d->pos_y - ray->map[1]) * ray->delta[1];
	}
}

static void	run_dda(t_data *d, t_ray *ray, t_wall *w)
{
	while (1)
	{
		if (ray->side_dist[0] < ray->side_dist[1])
		{
			ray->side_dist[0] += ray->delta[0];
			ray->map[0] += ray->step[0];
			ray->side = 0;
		}
		else
		{
			ray->side_dist[1] += ray->delta[1];
			ray->map[1] += ray->step[1];
			ray->side = 1;
		}
		if (d->map[ray->map[1]][ray->map[0]] == '1'
			|| d->map[ray->map[1]][ray->map[0]] == 'D')
		{
			w->is_door = (d->map[ray->map[1]][ray->map[0]] == 'D');
			break ;
		}
	}
}

static void	calc_wall_tex(t_data *d, t_ray *ray, t_wall *w)
{
	if (ray->side == 0)
	{
		w->perp = ray->side_dist[0] - ray->delta[0];
		w->wall_x = d->pos_y + w->perp * ray->dir[1];
		w->tex_idx = 2 + (ray->step[0] < 0);
	}
	else
	{
		w->perp = ray->side_dist[1] - ray->delta[1];
		w->wall_x = d->pos_x + w->perp * ray->dir[0];
		w->tex_idx = (ray->step[1] < 0);
	}
	if (w->is_door)
		w->tex_idx = 4;
	w->wall_x -= floor(w->wall_x);
	if ((ray->side == 0 && ray->dir[0] > 0) || (ray->side == 1
			&& ray->dir[1] < 0))
		w->wall_x = 1.0 - w->wall_x;
}

static void	calc_wall_height(t_wall *w)
{
	if (w->perp < 0.001)
		w->perp = 0.001;
	w->line_h = (int)(HEIGHT / w->perp);
	w->start = -w->line_h / 2 + HEIGHT / 2;
	if (w->start < 0)
		w->start = 0;
	w->end = w->line_h / 2 + HEIGHT / 2;
	if (w->end >= HEIGHT)
		w->end = HEIGHT - 1;
}

void	render_frame(t_data *d)
{
	int		x;
	t_ray	ray;
	t_wall	wall;
	double	cam;

	draw_bg(d);
	x = 0;
	while (x < WIDTH)
	{
		cam = 2 * x / (double)WIDTH - 1;
		init_ray(d, d->dir_x + d->plane_x * cam, d->dir_y + d->plane_y * cam,
			&ray);
		run_dda(d, &ray, &wall);
		calc_wall_tex(d, &ray, &wall);
		calc_wall_height(&wall);
		d->z_buffer[x] = wall.perp;
		draw_wall_slice(d, x, &wall);
		x++;
	}
	draw_minimap(d);
	draw_sprites(d);
}
