/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_read.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 23:48:52 by tkusama           #+#    #+#             */
/*   Updated: 2026/02/28 23:48:54 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

char	*read_file(int fd)
{
	char	buf[READ_SIZE + 1];
	char	*content;
	char	*tmp;
	int		bytes;

	content = ft_strdup("");
	if (!content)
		return (NULL);
	bytes = read(fd, buf, READ_SIZE);
	while (bytes > 0)
	{
		buf[bytes] = '\0';
		tmp = ft_strjoin(content, buf);
		free(content);
		content = tmp;
		if (!content)
			return (NULL);
		bytes = read(fd, buf, READ_SIZE);
	}
	if (bytes < 0)
	{
		free(content);
		return (NULL);
	}
	return (content);
}

static int	count_lines(const char *s)
{
	int	count;

	count = 1;
	while (*s)
	{
		if (*s == '\n')
			count++;
		s++;
	}
	return (count);
}

char	**split_lines(const char *s)
{
	char	**lines;
	int		total;
	int		i;
	int		len;
	int		start;

	total = count_lines(s);
	lines = malloc(sizeof(char *) * (total + 1));
	if (!lines)
		return (NULL);
	i = 0;
	start = 0;
	while (i < total)
	{
		len = 0;
		while (s[start + len] && s[start + len] != '\n')
			len++;
		lines[i] = ft_substr(s, start, len);
		if (!lines[i])
			return (free_split(lines), NULL);
		start += len + 1;
		i++;
	}
	lines[i] = NULL;
	return (lines);
}
