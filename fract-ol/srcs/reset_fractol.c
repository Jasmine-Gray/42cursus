/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reset_fractol.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 15:53:08 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/15 15:54:26 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

static void	set_fractol(t_data *data);

void	*init_fractol(t_data *data)
{
	data->mlx = mlx_init();
	if (data->mlx == NULL)
	{
		perror("Error mlx_init func");
		free_mlx(data);
		return (NULL);
	}
	data->window = mlx_new_window(data->mlx, WIDTH, HEIGHT, "data");
	if (data->window == NULL)
	{
		perror("Error mlx_new_window func");
		free_mlx(data);
		return (NULL);
	}
	data->img.img_ptr = mlx_new_image(data->mlx, WIDTH, HEIGHT);
	if (data->img.img_ptr == NULL)
	{
		perror("Error mlx_new_image func");
		free_mlx(data);
		return (NULL);
	}
	set_fractol(data);
	return (data);
}

static void	set_fractol(t_data *data)
{
	data->img.img_addr = mlx_get_data_addr(data->img.img_ptr,
			&data->img.bpp, &data->img.size_line, &data->img.endian);
	data->offset_x = 0.0;
	data->offset_y = 0.0;
	data->zoom = 200;
}
