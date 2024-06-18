/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 18:46:46 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/18 18:47:58 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

// int	main(int argc, char **argv)
// {
// 	int		fd;
// 	char	*line;
// 	int		line_count;
//
// 	line_count = 0;
// 	if (argc != 2)
// 	{
// 		printf("Usage: %s test1.txt\n", argv[0]);
// 		return (1);
// 	}
// 	fd = open(argv[1], O_RDONLY);
// 	if (fd == -1)
// 	{
// 		printf("Error opening file: %s\n", argv[1]);
// 		return (1);
// 	}
// 	while ((line = get_next_line(fd)) != NULL)
// 	{
// 		line_count++;
// 		printf("Line %d: %s\n", line_count, line);
// 		if (line[0] != '\0' && line[ft_strlen(line) - 1] != '\n')
// 			printf("\n");
// 		free(line);
// 	}
// 	close(fd);
// 	return (0);
// }

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("test1.txt", O_RDONLY);
	if (fd == -1)
	{
		printf("Failed to open file\n");
		return (1);
	}
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("%s", line);
		free(line);
	}
	close(fd);
	return (0);
}

// #include "get_next_line.h"
// #include <fcntl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
//
// int	main(int argc, char **argv)
// {
// 	int		fd;
// 	char	*line;
// 	int		line_count;
//
// 	line_count = 0;
// 	if (argc != 2)
// 	{
// 		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
// 		return (EXIT_FAILURE);
// 	}
// 	fd = open(argv[1], O_RDONLY);
// 	if (fd == -1)
// 	{
// 		perror("Error opening file");
// 		return (EXIT_FAILURE);
// 	}
// 	while ((line = get_next_line(fd)) != NULL)
// 	{
// 		line_count++;
// 		printf("Line %d: %s", line_count, line);
// 		if (*line && line[strlen(line) - 1] != '\n')
// 			printf("\n");
// 		free(line);
// 	}
// 	close(fd);
// 	return (EXIT_SUCCESS);
// }
