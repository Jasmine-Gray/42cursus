/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 22:25:02 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/28 22:42:12 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdio.h>

char	*read_file(int fd)
{
	char	*buf;
	char	*str;
	ssize_t	r;

	//char	*tmp;
	str = NULL;
	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buf)
		return (NULL);
	r=1;//追加
	// while ((r = read(fd, buf, BUFFER_SIZE)) > 0)//最悪のループ条件です！！
	while(r > 0)
	{
		r = read(fd, buf, BUFFER_SIZE);	
		buf[r] = '\0';
		str = ft_strjoin(str, buf);
		//tmp = ft_strjoin(str, buf);
		//free(str);
		//str = tmp;
		if (!str)
		{
			free(buf);
			return (NULL);
		}
		printf("buf=%s\n", buf);
		printf("str=%s\n", str);
		//改行を探してください！！！
	}
	free(buf);
	if (r == -1)
	{
		free(str);
		return (NULL);
	}
	printf("str===%s\n", str);
	return (str);
}

char	*get_a_line(char *str)
{
	size_t	i;
	size_t	flag;
	size_t	j;
	char	*buf;

	i = 0;
	j = 0;
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
	while (j < i + flag)
	{
		buf[j] = str[j];
		j++;
	}
	buf[j] = '\0';
	//ft_strlcpy(&buf, str, i + flag + 1);
	printf("get_a_line_buf=%s\n", buf);
	return (buf);
}

char	*read_next_line(char *str)
{
	size_t	i;
	char	*buf;

	i = 0;
	while (str[i] && str[i] != '\n')
		i++;
	//if (str[i] == '\n')
	//  i++;
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
  printf("i=%zu\n", i);
	ft_strlcpy(buf, &str[i], ft_strlen(str) - i + 1);
  printf("buf:%s\n", buf);
  printf("str[i]=%s\n", &str[i]);
	//printf("read_next_line_str1=%s\n", str);
	// free(str);
	//printf("read_next_line_str2=%s\n", str);
	return (buf);
}

char	*get_next_line(int fd)
{
	static char	*str;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	// if (!str || !*str)
	// {
	// 	free(str);
	// 	str = read_file(fd);
	// 	if (!str)
	//     return (NULL);
	// }
	//free(str);
	str = read_file(fd);
	if (!str)
		return (NULL);
	line = get_a_line(str);
	str = read_next_line(str);
	printf("gnl_line===%s\n", line);
	printf("gnl_str===%s\n", str);
	return (line);
}
