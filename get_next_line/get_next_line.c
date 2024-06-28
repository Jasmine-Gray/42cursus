/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 22:25:02 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/29 04:54:15 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*read_file(int fd, char *str)
{
	ssize_t	bytes;
	char	*buf;

	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buf)
		return (NULL);
	bytes = 1;
	while (bytes != 0 && !ft_strchr(str, '\n'))
	{
		bytes = read(fd, buf, BUFFER_SIZE);
		if (bytes < 0)
		{
			free(buf);
			return (NULL);
		}
    buf[bytes] = '\0';
		str = ft_strjoin(str, buf);
    //if (!str)
    //{
      //free(str);
      //return (NULL);
    //}
		// if (!ft_strchr(str, '\n'))
		// 	bytes = 0;
	}
  free(str);
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
	buf = (char *)malloc(sizeof(char) * i + 1);
	if (!buf)
		return (NULL);
  // printf("str=%s\n", str);
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

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	str = read_file(fd, str);
	//printf("gnl_str=%s\n", str);
	if (!str)
		return (NULL);
	result = get_line(str);
	str = skip_to_next_line(str);
	return (result);
}
