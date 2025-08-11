/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:38:43 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:38:45 by tkusama          ###   ########.fr       */
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
// #include <stdio.h>
// #include <string.h>
//
//  int	main(void)
//{
//	char	*str;
//	char	cmp;
//
//	str = "abcdefg";
//	cmp = 'e';
//	printf("ft_memchr:%s\n", ft_memchr(str, 0, 0));
//	printf("memchr:%s\n", memchr(str, 0, 0));
// }
