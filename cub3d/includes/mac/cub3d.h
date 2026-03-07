/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:09:31 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 17:09:31 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include "libft.h"
# include "mlx.h"
# include <fcntl.h>
# include <math.h>
# include <stdlib.h>
# include <unistd.h>

# define WIDTH 640
# define HEIGHT 480
# define READ_SIZE 4096

# define KEY_ESC 53
# define KEY_W 13
# define KEY_A 0
# define KEY_S 1
# define KEY_D 2
# define KEY_LEFT 123
# define KEY_RIGHT 124

# define MOVE_SPEED 0.05
# define ROT_SPEED 0.03

typedef struct s_img
{
	void		*img;
	char		*addr;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
}				t_img;

typedef struct s_tex
{
	void		*img;
	char		*addr;
	int			bpp;
	int			line_len;
	int			endian;
	int			width;
	int			height;
}				t_tex;

typedef struct s_keys
{
	int			w;
	int			a;
	int			s;
	int			d;
	int			left;
	int			right;
}				t_keys;

typedef struct s_data
{
	void		*mlx;
	void		*win;
	t_img		img;
	char		**map;
	double		pos_x;
	double		pos_y;
	double		dir_x;
	double		dir_y;
	double		plane_x;
	double		plane_y;
	int			floor_color;
	int			ceil_color;
	t_keys		keys;
	t_tex		tex[4];
}				t_data;

typedef struct s_ray
{
	double		dir[2];
	int			map[2];
	double		delta[2];
	double		side_dist[2];
	int			step[2];
	int			side;
}				t_ray;

typedef struct s_wall
{
	int			tex_idx;
	double		wall_x;
	double		perp;
	int			line_h;
	int			start;
	int			end;
}				t_wall;

typedef struct s_config
{
	char		*no_texture;
	char		*so_texture;
	char		*we_texture;
	char		*ea_texture;
	int			floor_color;
	int			ceil_color;
}				t_config;

typedef struct s_map
{
	char		**grid;
	int			width;
	int			height;
	double		player_x;
	double		player_y;
	char		player_dir;
}				t_map;

typedef struct s_game
{
	void		*mlx;
	void		*win;
	t_img		img;
	t_config	config;
	t_map		map;
}				t_game;

char			*read_file(int fd);
char			**split_lines(const char *s);
int				parse_cub_file(const char *filepath, t_game *game);
int				parse_config_line(char *line, t_config *config);
int				parse_rgb(char *str, int *color);
int				parse_map(char **lines, int start, t_map *map);
int				validate_map(t_map *map);

int				is_empty_line(char *line);
char			*skip_spaces(char *str);
void			print_error(char *msg);
void			free_config(t_config *config);
void			free_split(char **split);

void			my_pixel_put(t_img *img, int x, int y, int color);
void			draw_bg(t_data *d);
void			draw_wall_slice(t_data *d, int x, t_wall *w);
void			render_frame(t_data *d);
void			set_player_pos(t_map *map, int x, int y, char dir);
int				find_player_start(t_map *map);

int				key_press(int keycode, t_data *d);
int				key_release(int keycode, t_data *d);
int				close_window(t_data *d);
void			cleanup_data(t_data *d);
int				game_loop(t_data *d);

#endif
