/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 18:46:46 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/29 07:52:33 by mishimod         ###   ########.fr       */
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

	int i = 0;
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("i = %d : ",i++);
		printf("result===%s", line);
		free(line);
  }

	printf("BUFFER_SIZE=%d\n", BUFFER_SIZE);

	// line = get_next_line(fd);
	// printf("result===%s\n", line);
	// line = get_next_line(fd);
	// printf("result===%s\n", line);
	// line = get_next_line(fd);
	// printf("result===%s\n", line);
	// line = get_next_line(fd);
	// printf("result===%s\n", line);

	close(fd);
	return (0);
 }
