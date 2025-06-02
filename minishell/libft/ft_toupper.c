/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 20:38:57 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/09 14:32:16 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return (c - ('a' - 'A'));
	return (c);
}

// bool	check_toupper(void)
// {
// 	unsigned char	str_1[100];
// 	unsigned char	str_2[100];
// 	int				i;
//
// 	i = 0;
// 	while (i < 100)
// 	{
// 		str_1[i] = '\0';
// 		str_2[i] = '\0';
// 		i++;
// 	}
// 	if (ft_toupper(str_1[i]) != toupper(str_2[i]))
// 	{
// 		printf("ft_toupper=%d, toupper=%d\n", ft_toupper(i), toupper(i));
// 		return (false);
// 	}
// 	return (true);
// }
//
// int	main(void)
// {
// 	if (check_toupper())
// 	{
// 		printf("OK\n");
// 	}
// 	else
// 	{
// 		printf("ERROR\n");
// 	}
// }
//
