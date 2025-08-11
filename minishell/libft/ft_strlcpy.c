/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 21:41:33 by tkusama           #+#    #+#             */
/*   Updated: 2024/04/24 22:24:12 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	src_len;

	src_len = ft_strlen(src);
	i = 0;
	if (dstsize == 0)
		return (src_len);
	while (i < dstsize - 1 && src[i] != '\0')
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (src_len);
}

// #include <stdio.h>
// #include <string.h>

// int test_strlcpy(void)
// {
// 	char *str = "hello !";
// 	char buff1[0xF00];
// 	char buff2[0xF00];
// 	size_t r1;
// 	size_t r2;

// 	memset(buff1, 'A', 20);
// 	memset(buff2, 'A', 20);

// 	r1 = strlcpy(buff1, str, 2);
// 	r2 = ft_strlcpy(buff2, str, 2);

// 	printf("strlcpy:%zu, %s \n", r1, buff1);
// 	printf("ft_strlcpy:%zu, %s \n", r2, buff2);
// }

// int main(void)
// {
// 	test_strlcpy();
// 	return (0);
// }
