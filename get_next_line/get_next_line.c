/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 22:25:02 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/27 18:24:32 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdio.h>

char	*road_file(int fd)
{
	char	*buf;
	char	*str;
	char	*tmp;
	ssize_t	r;

	str = NULL;
	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buf)
		return (NULL);
	while ((r = read(fd, buf, BUFFER_SIZE)) > 0)
	{
		buf[r] = '\0';
		tmp = ft_strjoin(str, buf);
		free(str);
		str = tmp;
		if (!str)
		{
			free(buf);
			return (NULL);
		}
		//printf("buf=%s\n", buf);
		//printf("str=%s\n", str);
	}
	free(buf);
	if (r == -1)
	{
		free(str);
		return (NULL);
	}
	//printf("str===%s\n", str);
	return (str);
}

char	*get_a_line(char *str)
{
	size_t	i;
	size_t	flag;
	char	*buf;

	i = 0;
	if (!str[i])
		return (NULL);
	while (str[i] && str[i] != '\n')
		i++;
	flag = 0;
	if (str[i] == '\n')
		flag = 1;
	buf = (char *)malloc(sizeof(char) * (i + flag + 1));
	if (!buf)
		return (NULL);
	ft_strlcpy(buf, str, i + flag + 1);
	//printf("get_a_line_buf=%s\n", buf);
	return (buf);
}

char	*road_next_line(char *str)
{
	size_t	i;
	char	*buf;

	i = 0;
	while (str[i] && str[i] != '\n')
		i++;
	if (!str[i])
	{
		free(str);
		return (NULL);
	}
	buf = (char *)malloc(sizeof(char) * (ft_strlen(str) - i + 1));
	if (!buf)
	{
		free(str);
		return (NULL);
	}
	i++;
	ft_strlcpy(buf, &str[i], ft_strlen(str) - i + 1);
	//	printf("road_next_line_str1=%s\n", str);
	free(str);
	//	printf("road_next_line_str2=%s\n", str);
	return (buf);
}

char	*get_next_line(int fd)
{
	static char	*str;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!str || !*str)
	{
		free(str);
		str = road_file(fd);
		if (!str)
			return (NULL);
	}
	line = get_a_line(str);
	str = road_next_line(str);
	//	printf("gnl_line===%s\n", line);
	return (line);
}
