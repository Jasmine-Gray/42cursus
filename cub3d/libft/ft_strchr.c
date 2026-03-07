/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 21:54:57 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/06 15:43:23 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (1)
	{
		if (s[i] == (unsigned char)c)
			return ((char *)(&(s[i])));
		if (s[i] == '\0')
			return (NULL);
		i++;
	}
}
//#include <stdio.h>
//#include <string.h>
//
// int	main(void)
//{
//	char	*src;
//	char	*d1;
//	char	*d2;
//
//	src = "tlhere is so \0ma\0ny \0 \\0 in t\0his stri\0ng !\0\0\0\0";
//	d1 = strchr("test", '\0');
//	d2 = strchr("test", '\0');
//	printf("strchr: %s\n", d1);
//	printf("ft_strchr: %s\n", d2);
//	return (0);
//}
