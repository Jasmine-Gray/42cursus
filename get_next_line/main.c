/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 18:46:46 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/08 18:49:06 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "get_next_line.h"

int main(int argc, char **argv)
{
  int fd;
  char *line;
  int line_count = 0;

  if (argc != 2)
  {
    printf("Usage: %s test1.txt\n", argv[0]);
    return (1);
  }

  fd = open(argv[1], O_RDONLY);

  if (fd == -1)
  {
    printf("Error opening file: %s\n", argv[1]);
    return (1);
  }

  while ((line = get_next_line(fd)) != NULL)
  {
    line_count++;
    printf("Line %d: %s", line_count, line);
    if (line[0] != '\0' && line[strlen(line) - 1] != '\n')
      printf("\n");
    free(line);
  }
  close(fd);
  // open("./test1.txt", O_RDONLY);
  // open("./test2.txt", O_RDONLY);
  // open("./test3.txt", O_RDONLY);
  // open("./test4.txt", O_RDONLY);
  // open("./test5.txt", O_RDONLY);
  // open("./test6.txt", O_RDONLY);
  return (0);
}
