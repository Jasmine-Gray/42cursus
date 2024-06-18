/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 22:25:22 by mishimod          #+#    #+#             */
/*   Updated: 2024/06/18 19:12:03 by mishimod         ###   ########.fr       */
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

	src_len = ft_strlen(src);
	if (dstsize > 0)
	{
		while (*src && dstsize > 1)
		{
			*dst++ = *src++;
			dstsize--;
		}
		*dst = '\0';
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

// char	*ft_strjoin(char const *s1, char const *s2)
// {
// 	char	*str;
// 	size_t	len1;
// 	size_t	len2;
//
// 	if (!s1 && !s2)
// 		return (NULL);
// 	len1 = 0;
// 	if (s1)
// 		len1 = ft_strlen(s1);
// 	len2 = 0;
// 	if (s2)
// 		len2 = ft_strlen(s2);
// 	str = malloc(len1 + len2 + 1);
// 	if (!str)
// 		return (NULL);
// 	if (s1)
// 		ft_strlcpy(str, s1, len1 + 1);
// 	else
// 		str[0] = '\0';
// 	if (s2)
// 		ft_strlcpy(str + len1, s2, len2 + 1);
// 	return (str);
// }

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
