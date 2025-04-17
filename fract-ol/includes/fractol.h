/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fractol.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:57:10 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/17 16:45:56 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRACTOL_H
# define FRACTOL_H

# include "colour_code.h"
# include "ft_printf.h"
# include "img.h"
# include "key_code.h"
# include "libft.h"
# include "mlx.h"
# include <stdlib.h>

typedef struct s_data
{
	char	*type;
	char	*addr;
	void	*mlx;
	void	*window;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
	int		iter;
	double	zoom;
	double	offset_x;
	double	offset_y;
	double	first_julia;
	double	second_julia;
	t_img	img;
}			t_data;

# define WIDTH 1000
# define HEIGHT 1000
# define ITER_MAX 50

# define FT_ERROR "Error:\nUsage:\n./fractol mandelbrot\n./fractol julia <r> <i>\n"
# define FT_JULIA_ERROR "Error:\nExample:\n./fractol julia -0.2 0.7\n"

double		ft_atof(const char *str);
void		my_mlx_pixel_put(t_data *data, int x, int y, int color);
void		render_fractol(t_data *data);
void		render_mandelbrot(t_data *data);
void		render_julia(t_data *data);
void		put_pixel(t_img *img, double x, double y, int color);
void		*init_fractol(t_data *data);
void		free_mlx(t_data *data);
int			calc_color_gradient(int iterations);
int			allow_key(int key, t_data *data);
int			allow_mouse(int button, int x, int y, void *param);

#endif
