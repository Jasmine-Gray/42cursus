/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprite_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 19:50:47 by mishimod          #+#    #+#             */
/*   Updated: 2026/02/28 20:54:21 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

static void	calc_sprite_pos(t_data *d, t_sp_draw *sp)
{
	double	sx;
	double	sy;
	double	inv_det;

	sx = d->sprite.x - d->pos_x;
	sy = d->sprite.y - d->pos_y;
	inv_det = 1.0 / (d->plane_x * d->dir_y - d->dir_x * d->plane_y);
	sp->trans_x = inv_det * (d->dir_y * sx - d->dir_x * sy);
	sp->trans_y = inv_det * (-d->plane_y * sx + d->plane_x * sy);
	sp->screen_x = (int)((WIDTH / 2) * (1 + sp->trans_x / sp->trans_y));
	sp->h = abs((int)(HEIGHT / sp->trans_y));
	sp->w = abs((int)(HEIGHT / sp->trans_y));
	sp->start_y = (HEIGHT / 2 - sp->h / 2);
	if (sp->start_y < 0)
		sp->start_y = 0;
	sp->end_y = (HEIGHT / 2 + sp->h / 2);
	if (sp->end_y >= HEIGHT)
		sp->end_y = HEIGHT - 1;
	sp->start_x = (sp->screen_x - sp->w / 2);
	if (sp->start_x < 0)
		sp->start_x = 0;
	sp->end_x = (sp->screen_x + sp->w / 2);
	if (sp->end_x >= WIDTH)
		sp->end_x = WIDTH - 1;
}

static void	draw_sprite_stripe(t_data *d, t_sp_draw *sp, int stripe, t_img *tex)
{
	int				y;
	int				tex_x;
	int				tex_y;
	int				d_val;
	unsigned int	color;

	tex_x = (int)(256 * (stripe - (sp->screen_x - sp->w / 2))
			* tex->width / sp->w) / 256;
	if (sp->trans_y <= 0 || sp->trans_y >= d->z_buffer[stripe])
		return ;
	y = sp->start_y;
	while (y < sp->end_y)
	{
		d_val = y * 256 - HEIGHT * 128 + sp->h * 128;
		tex_y = ((d_val * tex->height) / sp->h) / 256;
		color = get_tex_pixel((t_tex *)tex, tex_x, tex_y);
		if ((color & 0x00FFFFFF) != 0xFF00FF)
			my_pixel_put(&d->img, stripe, y, color);
		y++;
	}
}

void	draw_sprites(t_data *d)
{
	t_sp_draw	sp;
	int			stripe;
	t_img		*tex;

	calc_sprite_pos(d, &sp);
	tex = &d->sprite.frames[d->sprite.cur_frame];
	stripe = sp.start_x;
	while (stripe < sp.end_x)
	{
		draw_sprite_stripe(d, &sp, stripe, tex);
		stripe++;
	}
}
