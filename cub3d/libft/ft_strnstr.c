/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 05:20:55 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/06 15:10:05 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;
	size_t	needle_len;

	needle_len = ft_strlen(needle);
	if (!needle || !(*needle))
		return ((char *)haystack);
	if (len == 0)
		return (NULL);
	if (ft_strlen(haystack) < needle_len || needle_len > len)
		return (NULL);
	i = 0;
	while (i <= len - needle_len)
	{
		j = 0;
		while (needle[j] && haystack[i + j] == needle[j])
			j++;
		if (needle[j] == '\0')
			return ((char *)&haystack[i]);
		i++;
	}
	return (NULL);
}

//#include <string.h>
//#include <xlocale.h>
//#include <stdio.h>
//
// int	main(void)
//{
//	char	*ptr_myfunc;
//	char	*ptr;
//
//	// const char	*largestring = "see FF your FF return ((FF now FF)");
//	// const char	*smallstring = "FF";
//	// const char	*haystack_myfunc = "see FF your FF return ((FF now FF)");
//	// const char	*needle_myfunc = "FF";
//  ptr = ft_strnstr("fake", ((void *)0), 3);
//	ptr_myfunc = ft_strnstr("fake", ((void *)0), 3);
//	printf("strnstr:%s\n", ptr);
//	printf("ft_strnstr:%s\n", ptr_myfunc);
//	return (0);
//}
