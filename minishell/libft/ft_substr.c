/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:40:33 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:41:29 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*str;
	size_t	s_len;
	size_t	size;

	size = 0;
	if (s == NULL)
		return (NULL);
	s_len = ft_strlen(s);
	if (s_len <= start || len == 0)
	{
		str = malloc(1);
		if (str == NULL)
			return (NULL);
		str[0] = '\0';
		return (str);
	}
	if (s_len > (start + len))
		size = len;
	else
		size = s_len - start;
	str = malloc(size + 1);
	if (str == NULL)
		return (NULL);
	ft_strlcpy(str, &(s[start]), size + 1);
	return (str);
}

// #include <stdio.h>
//
//  int	main(void)
//{
//	char	*s;
//
//	s = "hello";
//	printf("result:%s", ft_substr("hola", 4294967295, 18446744073709551615));
//	return (0);
// }
