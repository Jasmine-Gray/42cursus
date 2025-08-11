/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:36:22 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:36:24 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		((unsigned char *)s)[i] = '\0';
		i++;
	}
}

// #include <stdio.h>
// #include <string.h>

// void	print_array(const unsigned char *array, size_t size)
// {
// 	printf("Array contents: ");
// 	for (size_t i = 0; i < size; i++)
// 	{
// 		printf("%02X ", array[i]);
// 	}
// 	printf("\n");
// }

// int	main(void)
// {
// 	int	pass;

// 	unsigned char buffer[50];
// 	ft_bzero(buffer, sizeof(buffer));
// 	pass = 1;
// 	for (size_t i = 0; i < sizeof(buffer); i++)
// 	{
// 		if (buffer[i] != 0)
// 		{
// 			pass = 0;
// 			break ;
// 		}
// 	}
// 	if (pass)
// 	{
// 		printf("Test 1 Passed\n");
// 	}
// 	else
// 	{
// 		printf("Test 1 Failed\n");
// 	}
// 	print_array(buffer, sizeof(buffer));
// 	memset(buffer, 0xFF, sizeof(buffer));
// 	ft_bzero(buffer, 10);
// 	pass = 1;
// 	for (size_t i = 0; i < 10; i++)
// 	{
// 		if (buffer[i] != 0)
// 		{
// 			pass = 0;
// 			break ;
// 		}
// 	}
// 	for (size_t i = 10; i < sizeof(buffer); i++)
// 	{
// 		if (buffer[i] != 0xFF)
// 		{
// 			pass = 0;
// 			break ;
// 		}
// 	}
// 	if (pass)
// 	{
// 		printf("Test 2 Passed\n");
// 	}
// 	else
// 	{
// 		printf("Test 2 Failed\n");
// 	}
// 	print_array(buffer, sizeof(buffer));
// 	return (0);
// }
