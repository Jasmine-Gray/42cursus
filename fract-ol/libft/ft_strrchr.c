/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:30:17 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 18:27:21 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*last_c;

	last_c = NULL;
	while (1)
	{
		if (*s == (char)c)
			last_c = (char *)s;
		if (last_c && *s == '\0')
			return (last_c);
		if (!last_c && *s == '\0')
			return (NULL);
		s++;
	}
}

// bool	check_strrchr(void)
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
// 		if (ft_strrchr(str_1, a) != strrchr(str_2, a))
// 		{
// 			printf("ft_strrchr=%c, strrchr=%c\n", str_1[i], str_2[i]);
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }
//
// int	main(void)
// {
// 	if (check_strrchr())
// 		printf("OK\n");
// 	else
// 		printf("ERROR\n");
// }
