/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 15:11:41 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:21:31 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*str;

	str = (unsigned char *)s;
	while (n--)
	{
		*str++ = 0;
	}
	return ;
}

// bool	check_bzero(void)
// {
// 	const int	STR_SIZE = 42;
// 	char		str_1[STR_SIZE];
// 	char		str_2[STR_SIZE];
// 	int			i;
// 
// 	i = 0;
// 	while (i < STR_SIZE)
// 	{
// 		str_1[i] = '0';
// 		str_2[i] = '0';
// 		i++;
// 	}
// 	bzero(str_1, STR_SIZE);
// 	ft_bzero(str_2, STR_SIZE);
// 	str_1[STR_SIZE - 1] = '\0';
// 	str_2[STR_SIZE - 1] = '\0';
// 	printf("str_1 : %s\n", str_1);
// 	printf("str_2 : %s\n", str_2);
// 	while (i < STR_SIZE)
// 	{
// 		if (str_1[i] != str_2[i])
// 		{
// 			printf("bzero=%d, ft_bzero=%d\n", str_1[i], str_2[i]);
// 			return (false);
// 		}
// 	}
// 	return (true);
// }
// 
// int	main(void)
// {
// 	if (check_bzero())
// 	{
// 		printf("OK\n");
// 	}
// 	else
// 	{
// 		printf("ERROR\n");
// 	}
// }
