/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 22:25:22 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/06 14:40:02 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	src_len;

	src_len = 0;
	while (dstsize > 1 && *src != '\0')
	{
		*dst++ = *src++;
		src_len++;
		dstsize--;
	}
	if (dstsize > 0)
	{
		*dst = '\0';
	}
	while (*src != '\0')
	{
		src_len++;
		src++;
	}
	return (src_len);
}

char	*ft_strchr(const char *str, int c)
{
	char	cc;

	cc = (char)c;
	while (*str != '\0')
	{
		if (*str == cc)
			return ((char *)str);
		str++;
	}
	if (cc == '\0')
		return ((char *)str);
	return (NULL);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	size_t	len1;
	size_t	len2;

	if (!s1 && !s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	str = (char *)malloc(len1 + len2 + 1);
	if (!str)
		return (NULL);
	while (*s1)
		*str++ = *s1++;
	while (*s2)
		*str++ = *s2++;
	*str = 0;
	return (str - len1 - len2);
}
