/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 17:54:32 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:25:44 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char		*i;
	const unsigned char	*j;

	i = (unsigned char *)dst;
	j = (const unsigned char *)src;
	if (!dst && !src)
		return (NULL);
	while (n--)
		*i++ = *j++;
	return (dst);
}

// bool	check_memcpy(void)
// {
// 	const int	STR_SIZE = 42;
// 	char		src[STR_SIZE][STR_SIZE];
// 	char		dst_1[STR_SIZE][STR_SIZE];
// 	char		dst_2[STR_SIZE][STR_SIZE];
// 	int			i;
// 	int			j;
//
// 	i = 0;
// 	while (i < STR_SIZE)
// 	{
// 		j = 0;
// 		while (j < STR_SIZE)
// 		{
// 			src[i][j] = '\0';
// 			j++;
// 		}
// 		i++;
// 	}
// 	printf("src=%s\n", src[i]);
// 	strcpy(src[0], "");
// 	strcpy(src[1], "hello world");
// 	strcpy(src[2], "test");
// 	strcpy(src[3], "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
// 	i = 0;
// 	while (i < STR_SIZE)
// 	{
// 		ft_memcpy(dst_1[i], src[i], STR_SIZE);
// 		memcpy(dst_2[i], src[i], STR_SIZE);
// 		printf("result:%s\n", dst_1[i]);
// 		j = 0;
// 		while (j < STR_SIZE)
// 		{
// 			if (dst_1[i][j] != dst_2[i][j])
// 			{
// 				printf("ft_memcpy=%s, memcpy=%s\n", dst_1[i], dst_2[i]);
// 				return (false);
// 			}
// 			j++;
// 		}
// 		i++;
// 	}
// 	return (true);
// }
//
// int	main(void)
// {
// 	if (check_memcpy())
// 	{
// 		printf("OK\n");
// 	}
// 	else
// 	{
// 		printf("ERROR\n");
// 	}
// }
//
