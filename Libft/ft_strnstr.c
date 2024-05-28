/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:30:08 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:30:11 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;

	if (*needle == '\0')
		return ((char *)haystack);
	if (len == 0)
		return (NULL);
	i = 0;
	while (haystack[i] != '\0' && i < len)
	{
		j = 0;
		while (haystack[i + j] == needle[j] && needle[j] != '\0' && (i
				+ j) < len)
			j++;
		if (needle[j] == '\0')
			return ((char *)&haystack[i]);
		i++;
	}
	return (NULL);
}

// bool	check_strnstr(void)
// {
// 	char	str_1[100];
// 	char	str_2[100];
// 	//char	a[100];
// 	size_t	len;
// 	char	*res1;
// 	char	*res2;
// 
// 	//str_1[100] = {0};
// 	//str_2[100] = {0};
// 	//a[100] = "test";
// 	len = strlen("This is a test string for strnstr");
// 	strcpy(str_1, "This is a test string for strnstr");
// 	strcpy(str_2, "This is a test string for strnstr");
// 	res1 = ft_strnstr(str_1, NULL, len);
// 	res2 = strnstr(str_2, NULL, len);
// 	if (res1 == NULL && res2 == NULL)
// 		return (true);
// 	else if (res1 == NULL || res2 == NULL)
// 	{
// 		printf("ft_strnstr=%p, strnstr=%p\n", (void *)res1, (void *)res2);
// 		return (false);
// 	}
// 	else if (strcmp(res1, res2) != 0)
// 	{
// 		printf("ft_strnstr=%p, strnstr=%p\n", res1, res2);
// 		return (false);
// 	}
// 	return (true);
// }
// 
// int	main(void)
// {
// 	if (check_strnstr())
// 		printf("OK\n");
// 	else
// 		printf("ERROR\n");
// }
