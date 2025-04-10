/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fractol.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:57:10 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/10 19:21:00 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRACTOL_H
# define FRACTOL_H

# include "colour_code.h"
# include "ft_printf.h"
# include "libft.h"
# include "mlx.h"
# include "stdlib.h"

typedef struct s_data
{
	char	*type;
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
	double	first_julia;
	double	second_julia;
}			t_data;

# define WIDTH 1000
# define HEIGHT 1000
# define ITER_MAX 50

# define FT_ERROR "Error:\nUsage:\n./fractol mandelbrot\n./fractol julia <r> <i>\n"
# define FT_JULIA_ERROR "Error:\nExample:\n./fractol julia -0.2 0.7\n"

double		ft_atof(const char *str);
void		my_mlx_pixel_put(t_data *data, int x, int y, int color);
void		render_fractol(void *mlx, t_data *img, void *mlx_win);
int			calc_color_gradient(int iterations);

#endif
