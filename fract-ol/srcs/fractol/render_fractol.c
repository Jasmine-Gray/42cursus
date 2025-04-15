/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_fractol.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 23:03:40 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/15 17:46:48 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

static int	calc_mandelbrot(double x0, double y0)
{
	double	x;
	double	y;
	int		iter;
	double	x_new;

	x = 0.0;
	y = 0.0;
	iter = 0;
	while (x * x + y * y <= 4.0 && iter < ITER_MAX)
	{
		x_new = x * x - y * y + x0;
		y = 2 * x * y + y0;
		x = x_new;
		iter++;
	}
	if (iter < ITER_MAX)
		return (iter);
	return (0);
}

static int	calc_julia(double x0, double y0, double a, double b)
{
	double	x;
	double	y;
	int		iter;
	double	x_new;

	x = x0;
	y = y0;
	iter = 0;
	while (x * x + y * y <= 4.0 && iter < ITER_MAX)
	{
		x_new = x * x - y * y + a;
		y = 2 * x * y + b;
		x = x_new;
		iter++;
	}
	if (iter < ITER_MAX)
		return (iter);
	return (0);
}

void	render_mandelbrot(t_data *data)
{
	double	x;
	double	y;
	int		iter;

	y = 0;
	while (y < HEIGHT)
	{
		x = 0;
		while (x < WIDTH)
		{
			iter = calc_mandelbrot((x - WIDTH / 2 + data->offset_x)
					/ data->zoom, (y - HEIGHT / 2 + data->offset_y)
					/ data->zoom);
			if (iter == 0)
				put_pixel(&data->img, x, y, 0x000000);
			put_pixel(&data->img, x, y, calc_color_gradient(iter));
			x++;
		}
		y++;
	}
}

void	render_julia(t_data *data)
{
	double	x;
	double	y;
	int		iter;

	y = 0;
	while (y < HEIGHT)
	{
		x = 0;
		while (x < WIDTH)
		{
			iter = calc_julia((x - WIDTH / 2 + data->offset_x) / data->zoom, (y
						- HEIGHT / 2 + data->offset_y) / data->zoom,
					data->first_julia, data->second_julia);
			if (iter == 0)
				put_pixel(&data->img, x, y, 0x000000);
			put_pixel(&data->img, x, y, calc_color_gradient(iter));
			x++;
		}
		y++;
	}
}

void	render_fractol(t_data *data)
{
	mlx_destroy_image(data->mlx, data->img.img_ptr);
	data->img.img_ptr = mlx_new_image(data->mlx, WIDTH, HEIGHT);
	if (ft_strncmp(data->type, "julia", 5) == 0)
		render_julia(data);
	else
		render_mandelbrot(data);
	mlx_put_image_to_window(data->mlx, data->window, data->img.img_ptr,
		0, 0);
	data->img.img_addr = mlx_get_data_addr(data->img.img_ptr,
			&data->img.bpp, &data->img.size_line, &data->img.endian);
}
