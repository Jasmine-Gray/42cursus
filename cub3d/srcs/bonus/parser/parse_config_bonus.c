/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_config_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 15:47:19 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/21 10:46:17 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

static int	set_texture(char **dest, char *line)
{
	char	*path;
	int		fd;

	if (*dest)
	{
		print_error("Duplicate texture identifier");
		return (1);
	}
	path = skip_spaces(line);
	if (*path == '\0')
	{
		print_error("Missing texture path");
		return (1);
	}
	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		print_error("Texture file not found");
		return (1);
	}
	close(fd);
	*dest = ft_strdup(path);
	if (!*dest)
		return (1);
	return (0);
}

static int	set_color(int *dest, char *line)
{
	char	*value;

	if (*dest != -1)
	{
		print_error("Duplicate color identifier");
		return (1);
	}
	value = skip_spaces(line);
	if (*value == '\0')
	{
		print_error("Missing color value");
		return (1);
	}
	return (parse_rgb(value, dest));
}

static int	parse_texture(char *line, t_config *config)
{
	if (ft_strncmp(line, "NO", 2) == 0 && (line[2] == ' ' || line[2] == '\t'))
		return (set_texture(&config->no_texture, line + 2));
	if (ft_strncmp(line, "SO", 2) == 0 && (line[2] == ' ' || line[2] == '\t'))
		return (set_texture(&config->so_texture, line + 2));
	if (ft_strncmp(line, "WE", 2) == 0 && (line[2] == ' ' || line[2] == '\t'))
		return (set_texture(&config->we_texture, line + 2));
	if (ft_strncmp(line, "EA", 2) == 0 && (line[2] == ' ' || line[2] == '\t'))
		return (set_texture(&config->ea_texture, line + 2));
	return (-1);
}

int	parse_config_line(char *line, t_config *config)
{
	int	ret;

	line = skip_spaces(line);
	ret = parse_texture(line, config);
	if (ret != -1)
		return (ret);
	if (line[0] == 'F' && (line[1] == ' ' || line[1] == '\t'))
		return (set_color(&config->floor_color, line + 1));
	if (line[0] == 'C' && (line[1] == ' ' || line[1] == '\t'))
		return (set_color(&config->ceil_color, line + 1));
	return (-1);
}
