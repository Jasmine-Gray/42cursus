/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 17:09:31 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 21:33:03 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_BONUS_H
# define CUB3D_BONUS_H

# include "libft.h"
# include "mlx.h"
# include <fcntl.h>
# include <math.h>
# include <stdlib.h>
# include <unistd.h>

# define WIDTH 640
# define HEIGHT 480
# define READ_SIZE 4096

# define KEY_ESC 65307
# define KEY_W 119
# define KEY_A 97
# define KEY_S 115
# define KEY_D 100
# define KEY_E 101
# define KEY_LEFT 65361
# define KEY_RIGHT 65363

# define MOVE_SPEED 0.05
# define ROT_SPEED 0.03
# define MOUSE_SENSITIVITY 0.003
# define MINIMAP_TILE 8
# define MINIMAP_OFFSET 10

typedef struct s_img
{
	void		*img;
	char		*addr;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
	int			width;
	int			height;
}				t_img;

typedef struct s_tex
{
	void		*img;
	char		*addr;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
	int			width;
	int			height;
}				t_tex;

typedef struct s_sp_draw
{
	double		trans_x;
	double		trans_y;
	int			screen_x;
	int			h;
	int			w;
	int			start_x;
	int			end_x;
	int			start_y;
	int			end_y;
}				t_sp_draw;

typedef struct s_keys
{
	int			w;
	int			a;
	int			s;
	int			d;
	int			left;
	int			right;
}				t_keys;

typedef struct s_sprite
{
	t_img		frames[4];
	double		x;
	double		y;
	int			cur_frame;
	int			frame_count;
}				t_sprite;

typedef struct s_data
{
	void		*mlx;
	void		*win;
	t_img		img;
	t_img		door_tex;
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
	int			mouse_captured;
	t_tex		tex[4];
	t_sprite	sprite;
	double		z_buffer[WIDTH];
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
	int			is_door;
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
	int			height;
	double		player_x;
	double		player_y;
	char		player_dir;
}				t_map;

typedef struct s_game
{
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
int				get_tex_pixel(t_tex *tex, int x, int y);
void			draw_bg(t_data *d);
void			draw_wall_slice(t_data *d, int x, t_wall *w);
void			render_frame(t_data *d);
void			draw_minimap(t_data *d);
void			set_player_pos(t_map *map, int x, int y, char dir);
int				find_player_start(t_map *map);

int				key_press(int keycode, t_data *d);
int				key_release(int keycode, t_data *d);
int				mouse_press(int button, int x, int y, t_data *d);
int				close_window(t_data *d);
void			cleanup_data(t_data *d);
int				game_loop(t_data *d);
void			wrap_mouse_get_pos(t_data *d, int *x, int *y);
void			wrap_mouse_move(t_data *d, int x, int y);
void			wrap_mouse_hide(t_data *d);
void			wrap_mouse_show(t_data *d);
void			toggle_door(t_data *d);
int				load_textures(t_data *d, t_config *c);
int				load_sprite_frames(t_data *d);
void			update_sprite_animation(t_data *d);
void			draw_sprites(t_data *d);

#endif
