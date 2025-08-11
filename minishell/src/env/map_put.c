/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_put.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 08:27:06 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 08:27:15 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	map_put(t_map *map, const char *string)
{
	char	*name;
	char	*value;
	char	*equal_ptr;
	int		result;

	if (!string)
		return (-1);
	equal_ptr = ft_strchr(string, '=');
	if (!equal_ptr)
		return (map_set(map, string, NULL));
	name = ft_strndup(string, equal_ptr - string);
	if (!name)
		return (-1);
	value = equal_ptr + 1;
	result = map_set(map, name, value);
	free(name);
	return (result);
}
