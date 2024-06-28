/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 18:46:46 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/28 19:40:41 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("test1.txt", O_RDONLY);
	//fd = open("test2.txt", O_RDONLY);
	if (fd == -1)
	{
		printf("Failed to open file\n");
		return (1);
	}
	// while ((line = get_next_line(fd)) != NULL)
	// {
	// 	printf("result===%s", line);
	// 	free(line);
  // }
	printf("BUFFER_SIZE=%d\n", BUFFER_SIZE);
	line = get_next_line(fd);
	printf("result===%s", line);
	line = get_next_line(fd);
	printf("result===%s", line);
	line = get_next_line(fd);
	printf("result===%s", line);
	line = get_next_line(fd);
	printf("result===%s", line);
	close(fd);
	return (0);
 }
