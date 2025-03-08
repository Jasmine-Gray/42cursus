/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 20:12:32 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:27:10 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*i;
	unsigned char	uc;
	size_t			j;

	i = (unsigned char *)b;
	uc = (unsigned char)c;
	j = 0;
	while (len--)
	{
		i[j++] = uc;
	}
	return (b);
}

// bool	check_memset(void)
// {
//  	const int STR_SIZE = 42;
//  	char str_1[STR_SIZE];
//  	char str_2[STR_SIZE];
//  	int i;
//
//  	i = 0;
//  	while (i < STR_SIZE) {
//  		str_1[i] = '0';
//  		str_2[i] = '0';
//  		i++;
//  	}
//
//  	str_1[STR_SIZE - 1] = '\0';
//  	str_2[STR_SIZE - 1] = '\0';
//  	printf("Befor str_1: %s\n", str_1);
//  	printf("Befor str_2: %s\n", str_2);
//
//  	memset(str_1, '*', STR_SIZE);
//  	ft_memset(str_2, '*', STR_SIZE);
//
//  	str_1[STR_SIZE - 1] = '\0';
//  	str_2[STR_SIZE - 1] = '\0';
//  	printf("After str_1: %s\n", str_1);
//  	printf("After str_2: %s\n", str_2);
//
//  	while (i < STR_SIZE)
//  	{
//  		if (str_1[i] != str_2[i])
//  		{
//  			printf("ft_memset=%d, memset=%d\n", str_1[i], str_2[2]);
//  			return (false);
//  		}
//  		i++;
//  	}
//  	return (true);
// }
//
// int	main(void)
// {
// 	if (check_memset())
// 		printf("OK\n");
// 	else
// 		printf("ERROR\n");
// 	return (0);
// }
//
