/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:39:57 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:41:43 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	dstlen;
	size_t	srclen;
	size_t	i;

	dstlen = 0;
	srclen = 0;
	i = 0;
	srclen = ft_strlen(src);
	if (dst)
		dstlen = ft_strlen(dst);
	if (dstsize <= dstlen)
		return (dstsize + srclen);
	while (src[i] != '\0' && dstlen + i < dstsize - 1)
	{
		dst[dstlen + i] = src[i];
		i++;
	}
	dst[dstlen + i] = '\0';
	return (dstlen + srclen);
}

// #include <stdio.h>
// #include <string.h>
//
//  void	test_strlcat(void)
//{
//	char	*src;
//	char	buff1[1000] = "the cake is a lie !\0I'm hidden lol\r\n";
//	size_t	r1;
//	char	buff2[1000] = "the cake is a lie !\0I'm hidden lol\r\n";
//	size_t	r2;
//
//	// char *src_2;
//	src = "there is no stars in the sky";
//	r1 = strlcat(buff1, ((void *)0), 2);
//	r2 = ft_strlcat(buff2, ((void *)0), 2);
//	printf("strlcat:%zu, %s\n", r1, buff1);
//	printf("ft_strlcat:%zu, %s\n", r2, buff2);
// }
//
//  int	main(void)
//{
//	test_strlcat();
//	return (0);
// }
