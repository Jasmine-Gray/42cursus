/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 19:48:32 by mishimod          #+#    #+#             */
/*   Updated: 2024/07/04 20:06:24 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*read_file(int fd, char *str)
{
	ssize_t	bytes;
	char	*buf;
	char	*tmp;

	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buf)
		return (NULL);
	bytes = 1;
	while (bytes > 0 && (!str || !ft_strchr(str, '\n')))
	{
		bytes = read(fd, buf, BUFFER_SIZE);
		if (bytes < 0)
			return (free(buf), free(str), NULL);
		buf[bytes] = '\0';
		tmp = ft_strjoin(str, buf);
		free(str);
		str = tmp;
		if (!str)
			return (free(buf), NULL);
	}
	free(buf);
	return (str);
}

static char	*get_line(char *str)
{
	size_t	i;
	char	*buf;

	i = 0;
	if (!str[i])
		return (NULL);
	while (str[i] && str[i] != '\n')
		i++;
	if (str[i] == '\n')
		i++;
	buf = (char *)malloc(sizeof(char) * (i + 1));
	if (!buf)
		return (NULL);
	ft_strlcpy(buf, str, i + 1);
	return (buf);
}

static char	*skip_to_next_line(char *str)
{
	size_t	i;

	i = 0;
	if (!str[i] || !ft_strchr(str, '\n'))
		return (NULL);
	while (str[i] && str[i] != '\n')
		i++;
	if (ft_strchr(str, '\n'))
		i++;
	return (&(str[i]));
}

char	*get_next_line(int fd)
{
	static char	*str;
	char		*result;
	char		*tmp;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	str = read_file(fd, str);
	if (!str)
		return (NULL);
	result = get_line(str);
	tmp = str;
	str = ft_strdup(skip_to_next_line(str));
	free(tmp);
	return (result);
}
