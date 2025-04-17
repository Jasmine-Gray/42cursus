/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:53:51 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/17 16:45:35 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

static int	check_float(const char *str)
{
	int	has_dot;
	int	has_digit;

	has_dot = 0;
	has_digit = 0;
	if (str[0] == '0' && isdigit(str[1]))
		return (0);
	if (*str == '-' || *str == '+')
		str++;
	while (*str)
	{
		if (*str == '.')
		{
			if (has_dot)
				return (0);
			has_dot = 1;
		}
		else if (isdigit(*str))
			has_digit = 1;
		else
			return (0);
		str++;
	}
	return (has_digit);
}

static void	check_julia_arg(t_data *data, char **argv)
{
	data->type = argv[1];
	if (ft_strncmp(argv[1], "julia", 5) == 0)
	{
		if (check_float(argv[2]) && check_float(argv[3]))
		{
			data->first_julia = ft_atof(argv[2]);
			data->second_julia = ft_atof(argv[3]);
		}
		else
		{
			ft_putstr_fd(FT_JULIA_ERROR, STDERR_FILENO);
			exit(EXIT_FAILURE);
		}
	}
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (argc <= 1)
	{
		ft_putstr_fd(FT_ERROR, STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if ((argc == 2 && ft_strncmp(argv[1], "mandelbrot", 10) == 0
			&& ft_strlen(argv[1]) == 10) || (argc == 4 && ft_strncmp(argv[1],
				"julia", 5) == 0 && ft_strlen(argv[1]) == 5))
	{
		check_julia_arg(&data, argv);
		init_fractol(&data);
		render_fractol(&data);
		mlx_key_hook(data.window, allow_key, &data);
		 mlx_mouse_hook(data.window, allow_mouse, (void *)(&data));
		// mlx_hook(data.window, 17, 0L, cb_close_hook, &data);
		mlx_loop(data.mlx);
	}
	else
	{
		ft_putstr_fd(FT_ERROR, STDERR_FILENO);
		return (EXIT_FAILURE);
	}
}
