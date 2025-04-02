/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fractol.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:57:10 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/02 23:07:01 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRACTOL_H
# define FRACTOL_H

# include "ft_printf.h"
# include "libft.h"
# include "mlx.h"

double		ft_atof(const char *str);
# include <mlx.h>

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

void		my_mlx_pixel_put(t_data *data, int x, int y, int color);
void		render_fractol(void *mlx, t_data *img, void *mlx_win);

#endif
