/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 19:08:54 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/24 18:30:28 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	init_player_dir(t_data *d, char dir)
{
	d->dir_x = 0;
	d->dir_y = 0;
	d->plane_x = 0;
	d->plane_y = 0;
	if (dir == 'N')
	{
		d->dir_y = -1;
		d->plane_x = 0.66;
	}
	else if (dir == 'S')
	{
		d->dir_y = 1;
		d->plane_x = -0.66;
	}
	else if (dir == 'E')
	{
		d->dir_x = 1;
		d->plane_y = 0.66;
	}
	else if (dir == 'W')
	{
		d->dir_x = -1;
		d->plane_y = -0.66;
	}
}

static void	setup_data(t_data *d, t_game *game)
{
	d->map = game->map.grid;
	d->pos_x = game->map.player_x;
	d->pos_y = game->map.player_y;
	d->floor_color = game->config.floor_color;
	d->ceil_color = game->config.ceil_color;
	init_player_dir(d, game->map.player_dir);
}

static int	load_textures(t_data *d, t_config *c)
{
	char	*paths[4];
	int		i;

	paths[0] = c->no_texture;
	paths[1] = c->so_texture;
	paths[2] = c->we_texture;
	paths[3] = c->ea_texture;
	i = 0;
	while (i < 4)
	{
		d->tex[i].img = mlx_xpm_file_to_image(d->mlx, paths[i],
				&d->tex[i].width, &d->tex[i].height);
		if (!d->tex[i].img)
			return (print_error("Failed to load texture"), 1);
		d->tex[i].addr = mlx_get_data_addr(d->tex[i].img,
				&d->tex[i].bpp, &d->tex[i].line_len, &d->tex[i].endian);
		i++;
	}
	return (0);
}

static void	init_mlx(t_data *d)
{
	d->mlx = mlx_init();
	d->win = mlx_new_window(d->mlx, WIDTH, HEIGHT, "cub3D");
	d->img.img = mlx_new_image(d->mlx, WIDTH, HEIGHT);
	d->img.addr = mlx_get_data_addr(d->img.img, &d->img.bits_per_pixel,
			&d->img.line_length, &d->img.endian);
	mlx_hook(d->win, 2, 1L << 0, key_press, d);
	mlx_hook(d->win, 3, 1L << 1, key_release, d);
	mlx_hook(d->win, 17, 0, close_window, d);
	mlx_loop_hook(d->mlx, game_loop, d);
}

int	main(int argc, char **argv)
{
	t_game	game;
	t_data	d;

	if (argc != 2)
		return (print_error("Usage: ./cub3D <map.cub>"), 1);
	ft_memset(&game, 0, sizeof(t_game));
	ft_memset(&d, 0, sizeof(t_data));
	game.config.floor_color = -1;
	game.config.ceil_color = -1;
	if (parse_cub_file(argv[1], &game))
		return (free_config(&game.config), free_split(game.map.grid), 1);
	setup_data(&d, &game);
	init_mlx(&d);
	if (load_textures(&d, &game.config))
		return (free_config(&game.config), cleanup_data(&d), 1);
	free_config(&game.config);
	mlx_loop(d.mlx);
	return (0);
}
