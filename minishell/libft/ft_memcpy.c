/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:38:55 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:38:57 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t	i;

	i = 0;
	if (!(dst || src))
		return (dst);
	while (i < n)
	{
		((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
		i++;
	}
	return (dst);
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

//	int	main(void)
//{
// unsigned char	src[50];
// unsigned char	dst[50];
// int				pass;
// memset(src, 0xAB, sizeof(src));
// ft_memcpy(dst, src, sizeof(src));
// pass = 1;
// for (size_t i = 0; i < sizeof(src); i++)
// {
// 	if (dst[i] != src[i])
// 	{
// 		pass = 0;
// 		break ;
// 	}
// }
// if (pass)
// {
// 	printf("Test 1 Passed\n");
// }
// else
// {
// 	printf("Test 1 Failed\n");
// }
// print_array(dst, sizeof(dst));
// memset(src, 0xCD, sizeof(src));
// memset(dst, 0, sizeof(dst));
// ft_memcpy(dst, src, 10);
// pass = 1;
// for (size_t i = 0; i < 10; i++)
// {
// 	if (dst[i] != src[i])
// 	{
// 		pass = 0;
// 		break ;
// 	}
// }
// for (size_t i = 10; i < sizeof(dst); i++)
// {
// 	if (dst[i] != 0)
// 	{
// 		pass = 0;
// 		break ;
// 	}
// }
// if (pass)
// {
// 	printf("Test 2 Passed\n");
// }
// else
// {
// 	printf("Test 2 Failed\n");
// }
// print_array(dst, sizeof(dst));
// memset(src, 0xAB, sizeof(src));
// memset(dst, 0xCD, sizeof(dst));
// ft_memcpy(dst, src, 0);
// printf("Edge Case 1 (Zero Length Copy):\n");
// print_array(dst, sizeof(dst));
// ft_memcpy(NULL, NULL, 0);
// return (0);
//}
