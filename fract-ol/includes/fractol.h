/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fractol.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:57:10 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/10 16:00:42 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRACTOL_H
# define FRACTOL_H

# include "colour_code.h"
# include "ft_printf.h"
# include "libft.h"
# include "mlx.h"

typedef struct s_data
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}			t_data;

# define WIDTH 1000
# define HEIGHT 1000
# define ITER_MAX 50

double		ft_atof(const char *str);
void		my_mlx_pixel_put(t_data *data, int x, int y, int color);
void		render_fractol(void *mlx, t_data *img, void *mlx_win);
int			calc_color_gradient(int iterations);

#endif
