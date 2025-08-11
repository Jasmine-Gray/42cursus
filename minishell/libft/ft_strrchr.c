/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:40:24 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:41:26 by tkusama          ###   ########.fr       */
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
