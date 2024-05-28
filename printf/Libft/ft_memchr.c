/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:25:25 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/09 14:31:04 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*str;
	unsigned char		cc;

	str = (const unsigned char *)s;
	cc = (unsigned char)c;
	while (n > 0)
	{
		if (*str == cc)
			return ((void *)str);
		n--;
		str++;
	}
	return (NULL);
}

// bool	check_memchr(void)
// {
// 	unsigned char	str_1[100];
// 	unsigned char	str_2[100];
// 	const char		*s;
// 	size_t			i;
// 	size_t			n;
// 	void			*res1;
// 	void			*res2;
// 
// 	s = "Hello";
// 	n = strlen(s);
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
// 	i = 0;
// 	while (i < n)
// 	{
// 		res1 = ft_memchr(str_1, s[i], sizeof(str_1));
// 		res2 = memchr(str_2, s[i], sizeof(str_2));
// 		if (res1 != res2)
// 		{
// 			printf("ft_memchr=%p, memchr=%p\n", res1, res2);
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }
// 
// int	main(void)
// {
// 	if (check_memchr())
// 		printf("OK\n");
// 	else
// 		printf("ERROR\n");
// 	return (0);
// }
