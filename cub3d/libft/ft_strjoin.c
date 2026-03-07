/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 21:56:42 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/01 22:17:30 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	size_t	s1_len;
	size_t	s2_len;
	size_t	i;

	i = 0;
	if (s1 == NULL || s2 == NULL)
		return (NULL);
	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	str = malloc(s1_len + s2_len + 1);
	if (str == NULL)
		return (NULL);
	while (i < s1_len)
	{
		str[i] = s1[i];
		i++;
	}
	ft_strlcpy(&(str[i]), s2, s2_len + 1);
	return (str);
}

//#include <stdio.h>
//
//int	main(void)
//{
//	char	*s1;
//	char	*s2;
//	char	*result;
//
//	s1 = "hello";
//	s2 = "world";
//	result = ft_strjoin(s1, s2);
//	printf("result:%s", result);
//	return (0);
//}
