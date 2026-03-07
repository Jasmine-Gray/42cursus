/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:15:26 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/10 20:09:10 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalpha(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}

// #include <stdio.h>
// #include <ctype.h>

// int	main(void)
// {
// 	printf("alpha_1result:%d\n", isalpha('c'));
// 	printf("isalpha_2result:%d\n", isalpha('2'));
// 	printf("ft_isalpha_1result:%d\n", ft_isalpha('c'));
// 	printf("ft_isalpha_2result:%d\n", ft_isalpha('2'));
// }
