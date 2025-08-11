/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:36:52 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:36:54 by tkusama          ###   ########.fr       */
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
