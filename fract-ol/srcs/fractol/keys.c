/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 18:13:53 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/17 16:44:55 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cb_keys_mouse_hook.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 22:22:50 by tkusama           #+#    #+#             */
/*   Updated: 2024/12/21 03:20:05 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

int	allow_key(int key, t_data *data)
{
	if (key == KEY_ESC)
	{
		free_mlx(data);
		exit(EXIT_SUCCESS);
	}
	else if (key == KEY_LEFT)
		data->offset_x -= data->zoom * 0.1;
	else if (key == KEY_RIGHT)
		data->offset_x += data->zoom * 0.1;
	else if (key == KEY_UP)
		data->offset_y -= data->zoom * 0.1;
	else if (key == KEY_DOWN)
		data->offset_y += data->zoom * 0.1;
	render_fractol(data);
	return (0);
}

int	allow_mouse(int button, int x, int y, void *param)
{
		t_data	*data;

		(void)x;
		(void)y;
		data = (t_data *)param;
		if (data == NULL)
		{
			perror("Error: data is NULL in cb_mouse_hook\n");
			return (0);
		}
		if (button == MOUSE_UP)
			data->zoom *= 0.95;
		else if (button == MOUSE_DOWN)
			data->zoom /= 0.95;
		render_fractol(data);
		return (0);
}
