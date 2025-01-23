/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:30:39 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:30:48 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	unsigned char	*str;
	size_t			size;

	if (!s)
		return (NULL);
	size = ft_strlen(s);
	if (start >= size || len == 0)
	{
		str = malloc(1);
		if (!str)
			return (NULL);
		str[0] = '\0';
		return ((char *)str);
	}
	if (start + len <= size)
		size = len;
	str = malloc(size + 1);
	if (!str)
		return (NULL);
	ft_strlcpy((char *)str, s + start, size + 1);
	return ((char *)str);
}

//s[] = "hinako"
//start = 8;
//len = 5;
//str[] == "hinak"
