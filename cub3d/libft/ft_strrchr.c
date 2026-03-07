/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 21:22:57 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/09 10:58:19 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	size_t	i;

	i = ft_strlen(s) + 1;
	while (i > 0)
	{
		if (s[i - 1] == (char)c)
			return ((char *)&s[i - 1]);
		i--;
	}
	return (NULL);
}

// #include <stdio.h>
// #include <string.h>

// int main (void)
// {
// 	char *str = "abcdefg";
// 	char *result;
// 	char *result2;

// 	result = ft_strrchr(str, '\0');
// 	result2 = strrchr(str, '\0');
// 	printf("%p\n", result);
// 	printf("%p\n", result2);
// 	return (0);
// }
