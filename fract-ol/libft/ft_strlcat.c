/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 22:21:59 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:29:24 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	unsigned int	dst_len;
	unsigned int	src_len;
	size_t			n;

	src_len = ft_strlen(src);
	if (!dst && dstsize == 0)
		return (src_len);
	dst_len = ft_strlen(dst);
	if (dstsize <= dst_len)
		return (dstsize + src_len);
	else
	{
		while (*dst != '\0')
		{
			dst++;
		}
		n = 0;
		while (*src != '\0' && n < dstsize - dst_len - 1)
		{
			*dst++ = *src++;
			n++;
		}
		*dst = '\0';
		return (dst_len + src_len);
	}
}

// bool	check_strlcat(void)
// {
// 	char	dst_1[100] = "Hello ";
// 	char	dst_2[100] = "Hello ";
// 	char	src_1[100] = "World ";
// 	char	src_2[100] = "World ";
// 	size_t	len_1;
// 	size_t	len_2;
//
// 	len_1 = ft_strlcat(dst_1, src_1, sizeof(dst_1));
// 	len_2 = ft_strlcat(dst_2, src_2, sizeof(dst_2));
// 	if (len_1 != len_2)
// 	{
// 		printf("ft_strlcat=%zu, strlcat=%zu\n", len_1, len_2);
// 		return (false);
// 	}
// 	if (strcmp(dst_1, dst_2) != 0)
// 	{
// 		printf("ft_strlcat=%s, strlcat=%s\n", dst_1, dst_2);
// 		return (false);
// 	}
// 	return (true);
// }
//
// int	main(void)
// {
// 	if (check_strlcat())
// 	{
// 		printf("OK\n");
// 	}
// 	else
// 	{
// 		printf("ERROR\n");
// 	}
// }
