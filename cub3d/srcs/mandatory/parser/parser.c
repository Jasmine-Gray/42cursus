/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 14:22:31 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 22:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	check_extension(const char *filepath)
{
	char	*ext;

	ext = ft_strrchr(filepath, '.');
	if (!ext || ft_strncmp(ext, ".cub", 5) != 0)
	{
		print_error("Invalid file extension");
		return (1);
	}
	return (0);
}

static int	is_config_complete(t_config *conf)
{
	if (!conf->no_texture || !conf->so_texture
		|| !conf->we_texture || !conf->ea_texture
		|| conf->floor_color == -1
		|| conf->ceil_color == -1)
	{
		print_error("Missing configuration element");
		return (0);
	}
	return (1);
}

static int	parse_lines(char **lines, t_game *game)
{
	int	i;
	int	ret;

	i = -1;
	while (lines[++i])
	{
		if (is_empty_line(lines[i]))
			continue ;
		ret = parse_config_line(lines[i], &game->config);
		if (ret == 1)
			return (1);
		if (ret == -1)
		{
			if (!is_config_complete(&game->config))
				return (1);
			return (parse_map(lines, i, &game->map));
		}
	}
	if (!is_config_complete(&game->config))
		return (1);
	return (print_error("No map found"), 1);
}

int	parse_cub_file(const char *filepath, t_game *game)
{
	int		fd;
	char	*content;
	char	**lines;
	int		ret;

	if (check_extension(filepath))
		return (1);
	fd = open(filepath, O_RDONLY);
	if (fd < 0)
		return (print_error("Cannot open file"), 1);
	content = read_file(fd);
	close(fd);
	if (!content)
		return (print_error("Failed to read file"), 1);
	lines = split_lines(content);
	free(content);
	if (!lines)
		return (1);
	ret = parse_lines(lines, game);
	free_split(lines);
	return (ret);
}
