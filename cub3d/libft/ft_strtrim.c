/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 16:11:32 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/09 08:31:27 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*trim;
	size_t	start;
	size_t	end;

	start = 0;
	if (s1 == NULL || set == NULL)
		return (NULL);
	end = ft_strlen(s1);
	while (s1[start] != '\0' && ft_strchr(set, s1[start]))
		start++;
	while (end > start && ft_strchr(set, s1[end - 1]))
		end--;
	trim = malloc((sizeof(char)) * (end - start + 1));
	if (trim == NULL)
		return (NULL);
	ft_strlcpy(trim, &s1[start], end - start + 1);
	trim[end - start] = '\0';
	return (trim);
}

//#include <stdio.h>
//
// int	main(void)
//{
//	char		*s1;
//	char const	*set;
//
//	s1 = "lalhello world!ll";
//	set = "la";
//	printf("result:%s", ft_strtrim(s1, set));
//	return (0);
//}
