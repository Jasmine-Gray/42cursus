/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_color_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 11:03:42 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 21:31:34 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

static int	is_valid_rgb_str(char *str)
{
	int	comma_count;

	comma_count = 0;
	while (*str)
	{
		if (*str == ',')
			comma_count++;
		else if (!ft_isdigit(*str) && *str != ' ' && *str != '\t')
			return (0);
		str++;
	}
	return (comma_count == 2);
}

static int	count_digits(char *str)
{
	int	count;

	while (*str == ' ' || *str == '\t')
		str++;
	count = 0;
	while (ft_isdigit(str[count]))
		count++;
	return (count);
}

static int	extract_value(char *str, int *val)
{
	int	digits;

	digits = count_digits(str);
	if (digits == 0 || digits > 3)
	{
		print_error("Color value out of range (0-255)");
		return (1);
	}
	*val = ft_atoi(str);
	if (*val < 0 || *val > 255)
	{
		print_error("Color value out of range (0-255)");
		return (1);
	}
	return (0);
}

int	parse_rgb(char *str, int *color)
{
	char	**parts;
	int		r;
	int		g;
	int		b;

	if (!is_valid_rgb_str(str))
	{
		print_error("Invalid color format");
		return (1);
	}
	parts = ft_split(str, ',');
	if (!parts)
		return (1);
	if (!parts[0] || !parts[1] || !parts[2] || parts[3])
	{
		print_error("Color must have exactly 3 values");
		free_split(parts);
		return (1);
	}
	if (extract_value(parts[0], &r) || extract_value(parts[1], &g)
		|| extract_value(parts[2], &b))
		return (free_split(parts), 1);
	*color = (r << 16) | (g << 8) | b;
	free_split(parts);
	return (0);
}
