/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 15:36:35 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/09 16:07:11 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char			*str;
	unsigned int	len;
	unsigned int	i;

	i = 0;
	if (!s || !f)
		return (NULL);
	len = ft_strlen(s);
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	while (i < len)
	{
		str[i] = f(i, s[i]);
		i++;
	}
	str[len] = '\0';
	return (str);
}

//#include <stdio.h>

//char	shift_char(unsigned int i, char c)
//{
//	return (c + i % 26);
//}
//int	main(void)
//{
//	char const	*original;
//	char		*modified;
//
//	original = "hello";
//	modified = ft_strmapi(original, shift_char);
//	printf("Original: %s\nModified: %s\n", original, modified);
//	free(modified);
//	return (0);
//}
