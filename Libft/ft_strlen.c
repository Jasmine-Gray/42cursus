/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 12:26:06 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:29:36 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
	{
		i++;
	}
	return (i);
}

// bool	check_strlen(void)
// {
// 	int		i;
// 	int		j;
// 	char	s[100][100];

// 	i = 0;
// 	j = 0;
// 	while (i < 100)
// 	{
// 		while (j < 100)
// 		{
// 			s[i][j] = '\0';
// 			j++;
// 		}
// 		i++;
// 	}
// 	strcpy(s[0], "Hello World!");
// 	strcpy(s[1], "test");
// 	i = 0;
// 	while (i < 100)
// 	{
// 		if (ft_strlen(s[i]) != strlen(s[i]))
// 		{
// 			printf("ft_strlen=%lu, strlen=%lu\n", ft_strlen(s[i]),
// 				strlen(s[i]));
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }

// int	main(void)
// {
// 	if (check_strlen())
// 	{
// 		printf("OK\n");
// 	}
// 	else
// 	{
// 		printf("ERROR\n");
// 	}
// }
