/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:28:41 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:28:46 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	char	cc;

	cc = (char)c;
	while (*s != '\0')
	{
		if (*s == cc)
			return ((char *)s);
		s++;
	}
	if (cc == '\0')
		return ((char *)s);
	return (NULL);
}

// bool	check_strchr(void)
// {
// 	char		str_1[100];
// 	char		str_2[100];
// 	const char	a;
// 	int			i;
// 
// 	a = 'A';
// 	i = 0;
// 	while (i < 100)
// 	{
// 		str_1[i] = '\0';
// 		str_2[i] = '\0';
// 		i++;
// 	}
// 	strcpy(&str_1[0], "");
// 	strcpy(&str_2[0], "");
// 	strcpy(&str_1[1], "hello world");
// 	strcpy(&str_2[1], "hello world");
// 	strcpy(&str_1[2], "test");
// 	strcpy(&str_2[2], "test");
// 	strcpy(&str_1[3], "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
// 	strcpy(&str_2[3], "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
// 	i = 0;
// 	while (i < 100)
// 	{
// 		if (ft_strchr(str_1, a) != strchr(str_2, a))
// 		{
// 			printf("ft_strchr=%c, strchr=%c\n", str_1[i], str_2[i]);
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }
// 
// int	main(void)
// {
// 	if (check_strchr())
// 		printf("OK\n");
// 	else
// 		printf("ERROR\n");
// }
