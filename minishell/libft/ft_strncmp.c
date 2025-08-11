/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:40:09 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:41:32 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		if (s1[i] != s2[i])
			return ((int)((unsigned char)s1[i] - (unsigned char)s2[i]));
		else if (s1[i] == '\0' || s2[i] == '\0')
			return (0);
		i++;
	}
	return (0);
}

// #include <stdio.h>
// #include <string.h>
//
//  int	main(void)
//{
//	char	*s1;
//	char	*s2;
//	char	*big;
//	char	*little;
//	size_t	size;
//
//	big = "omg1";
//	little = "omg3|||||||";
//	size = 4;
//	s1 = "\200";
//	s2 = "\0";
//	printf("ft_strncmp: %d\n", ft_strncmp(s1, s2, 3));
//	printf("strncmp: %d\n", strncmp(s1, s2, 3));
//	printf("ft_strncmp: %d\n", ft_strncmp(big, little, 6));
//	printf("strncmp: %d\n", strncmp(big, little, 6));
//	return (0);
// }
