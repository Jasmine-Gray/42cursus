/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:36:57 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/21 14:51:29 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

void	print_error(char *msg)
{
	ft_putstr_fd("Error\n", 2);
	ft_putstr_fd(msg, 2);
	ft_putstr_fd("\n", 2);
}

int	is_empty_line(char *line)
{
	if (!line)
		return (1);
	if (*line == '\0')
		return (1);
	while (*line)
	{
		if (*line != ' ' && *line != '\t')
		{
			return (0);
		}
		line++;
	}
	return (1);
}

char	*skip_spaces(char *str)
{
	while (*str)
	{
		if (*str != ' ' && *str != '\t')
		{
			return (str);
		}
		str++;
	}
	return (str);
}

void	free_config(t_config *config)
{
	if (!config)
		return ;
	free(config->no_texture);
	free(config->so_texture);
	free(config->we_texture);
	free(config->ea_texture);
	config->no_texture = NULL;
	config->so_texture = NULL;
	config->we_texture = NULL;
	config->ea_texture = NULL;
}

void	free_split(char **split)
{
	int	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}
