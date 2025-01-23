/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:29:51 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:29:55 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char	*str1;
	unsigned char	*str2;
	size_t			i;

	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	i = 0;
	while (i < n)
	{
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
		else if (str1[i] == '\0' && str2[i] == '\0')
			return (0);
		i++;
	}
	return (0);
}

// bool	check_strncmp(void)
// {
// 	unsigned char	str_1[100];
// 	unsigned char	str_2[100];
// 	const char		*s;
// 	size_t			i;
// 	size_t			n;
// 
// 	s = "Hello";
// 	i = 0;
// 	memset(str_1, 0, sizeof(str_1));
// 	memset(str_2, 0, sizeof(str_2));
// 	strcpy((char *)str_1, "");
// 	strcpy((char *)str_2, "");
// 	strcpy((char *)(str_1 + 1), "Hello World");
// 	strcpy((char *)(str_2 + 1), "Hello World");
// 	strcpy((char *)(str_1 + 2), "aaaaaaaaaaaaaaaa");
// 	strcpy((char *)(str_2 + 2), "aaaaaaaaaaaaaaaa");
// 	strcpy((char *)(str_1 + 3), "     ");
// 	strcpy((char *)(str_2 + 3), "     ");
// 	strcpy((char *)(str_1 + 4), "test");
// 	strcpy((char *)(str_2 + 4), "test");
// 	while (n <= strlen(s))
// 	{
// 		i = 0;
// 		while (i < 100)
// 		{
// 			if (ft_strncmp((const char *)(str_1 + i), s,
// 					n) != strncmp((const char *)(str_2 + i), s, n))
// 			{
// 				printf("ft_strncmp=%s, strncmp=%s\n", str_1 + i, str_2 + i);
// 				return (false);
// 			}
// 			i++;
// 		}
// 		n++;
// 	}
// 	return (true);
// }
// 
// int	main(void)
// {
// 	if (check_strncmp())
// 		printf("OK\n");
// 	else
// 		printf("ERROR\n");
// }
