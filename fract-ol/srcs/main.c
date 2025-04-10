/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 18:53:51 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/10 19:35:29 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

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
	void	*mlx;
	void	*mlx_win;
	t_data	img;

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
		mlx = mlx_init();
		mlx_win = mlx_new_window(mlx, WIDTH, HEIGHT, "Hello world!");
		render_fractol(mlx, &img, mlx_win);
		mlx_loop(mlx);
	}
	else
	{
		ft_putstr_fd(FT_ERROR, STDERR_FILENO);
		return (EXIT_FAILURE);
	}
}
