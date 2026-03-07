/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 18:03:30 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/09 09:29:03 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	size_t	i;

	i = 0;
	if (!(dst || src))
		return (dst);
	if (dst >= src)
	{
		while (len > 0)
		{
			((unsigned char *)dst)[len - 1] = ((unsigned char *)src)[len - 1];
			len--;
		}
	}
	else
	{
		while (i < len)
		{
			((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
			i++;
		}
	}
	return (dst);
}

//#include <stdio.h>
//#include <string.h>

// int	main(void)
// {
// 	char	s1[] = "12345678901234567890";

// 	// char	s2[] = "12345678901234567890";
// 	//
// 	// printf("before copied:%s\n\n", s1);
// 	// ft_memmove(s1, s1 + 3, 5);
// 	// printf("after copied:%s\n", s1);
// 	// //
// 	printf("before copied   :%s\n\n", s1);
// 	memmove(((void *)0), ((void *)0), 0);
// 	printf("after copied   :%s\n\n", s1);
// 	// printf("before copied ft:%s\n\n", s2);
// 	// ft_memmove(((void *)0), ((void *)0), 0);
// 	// printf("after copied ft:%s\n\n", s2);
// 	//
// 	// printf("before copied:%s\n\n", s1);
// 	// ft_memmove(s1 + 3, s1, 5);
// 	// printf("after copied:%s\n", s1);
// 	return (0);
// }
