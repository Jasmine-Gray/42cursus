/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 20:45:53 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:29:29 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	src_len;

	src_len = 0;
	while (dstsize > 1 && *src != '\0')
	{
		*dst++ = *src++;
		src_len++;
		dstsize--;
	}
	if (dstsize > 0)
	{
		*dst = '\0';
	}
	while (*src != '\0')
	{
		src_len++;
		src++;
	}
	return (src_len);
}

//  bool	check_strlcpy(void)
//  {
//  	char		dst_1[100];
//  	char		dst_2[100];
//  	const char	*src = "Hello World";
//  	size_t		len_1;
//  	size_t		len_2;
//  
//  	len_1 = ft_strlcpy(dst_1, src, sizeof(dst_1));
//  	len_2 = strlcpy(dst_2, src, sizeof(dst_2));
//  	if (len_1 != len_2)
//  	{
//  		printf("ft_strlcpy=%zu, strlcpy=%zu\n", len_1, len_2);
//  		return (false);
//  	}
//  	if (strcmp(dst_1, dst_2) != 0)
//  	{
//  		printf("ft_strlcpy=%s, strlcpy=%s\n", dst_1, dst_2);
//  		return (false);
//  	}
//  	return (true);
//  }
//  
//  int	main(void)
//  {
//  	if (check_strlcpy())
//  	{
//  		printf("OK\n");
//  	}
//  	else
//  	{
//  		printf("ERROR\n");
//  	}
//  }
//  
