/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:39:01 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:39:03 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *b, int c, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		((unsigned char *)b)[i] = (unsigned char)c;
		i++;
	}
	return (b);
}

// test case
// #include <stdio.h>
// #include <string.h>

// void	print_array(const char *array, size_t size)
// {
// 	printf("Array contents: ");
// 	for (size_t i = 0; i < size; i++)
// 	{
// 		printf("%02X ", (unsigned char)array[i]);
// 	}
// 	printf("\n");
// }

// int	main(void)
// {
// 	int		pass;
// 	char	large_buffer[1000];

// 	char buffer[100];
// 	ft_memset(buffer, 0xAA, sizeof(buffer));
// 	pass = 1;
// 	for (size_t i = 0; i < sizeof(buffer); i++)
// 	{
// 		if (buffer[i] != (char)0xAA)
// 		{
// 			pass = 0;
// 			break ;
// 		}
// 	}
// 	if (pass)
// 	{
// 		printf("Test 1 Passed\n");
// 		print_array(buffer, sizeof(buffer));
// 	}
// 	else
// 	{
// 		printf("Test 1 Failed\n");
// 		print_array(buffer, sizeof(buffer));
// 	}
// 	ft_memset(buffer, 0xAA, sizeof(buffer));
// 	ft_memset(buffer, 0xBB, 5);
// 	pass = 1;
// 	for (size_t i = 0; i < 5; i++)
// 	{
// 		if (buffer[i] != (char)0xBB)
// 		{
// 			pass = 0;
// 			break ;
// 		}
// 	}
// 	for (size_t i = 5; i < sizeof(buffer); i++)
// 	{
// 		if (buffer[i] != (char)0xAA)
// 		{
// 			pass = 0;
// 			break ;
// 		}
// 	}
// 	if (pass)
// 	{
// 		printf("Test 2 Passed\n");
// 		print_array(buffer, sizeof(buffer));
// 	}
// 	else
// 	{
// 		printf("Test 2 Failed\n");
// 		print_array(buffer, sizeof(buffer));
// 	}
// 	ft_memset(large_buffer, 0x00, sizeof(large_buffer));
// 	pass = 1;
// 	for (size_t i = 0; i < sizeof(large_buffer); i++)
// 	{
// 		if (large_buffer[i] != 0x00)
// 		{
// 			pass = 0;
// 			break ;
// 		}
// 	}
// 	if (pass)
// 	{
// 		printf("Test 3 Passed\n");
// 		print_array(large_buffer, sizeof(large_buffer));
// 	}
// 	else
// 	{
// 		printf("Test 3 Failed\n");
// 		print_array(large_buffer, sizeof(large_buffer));
// 	}
// 	return (0);
// }
