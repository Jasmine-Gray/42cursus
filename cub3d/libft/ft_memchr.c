/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 17:26:10 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/06 15:55:34 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*s_cast;

	s_cast = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		if (s_cast[i] == (unsigned char)c)
			return (&(s_cast[i]));
		i++;
	}
	return (NULL);
}
//#include <stdio.h>
//#include <string.h>
//
// int	main(void)
//{
//	char	*str;
//	char	cmp;
//
//	str = "abcdefg";
//	cmp = 'e';
//	printf("ft_memchr:%s\n", ft_memchr(str, 0, 0));
//	printf("memchr:%s\n", memchr(str, 0, 0));
//}
