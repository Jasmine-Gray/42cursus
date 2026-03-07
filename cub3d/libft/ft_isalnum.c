/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 20:43:31 by tkusama           #+#    #+#             */
/*   Updated: 2024/08/30 16:32:11 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

int	ft_isalnum(int c)
{
	if (ft_isalpha(c) || ft_isdigit(c))
		return (1);
	return (0);
}

// #include <ctype.h>
// #include <stdio.h>

// int	main(void)
// {
// 	printf("isalnum_1_result:%d\n", isalnum('c'));
// 	printf("isalnum_2_result:%d\n", isalnum('!'));
// 	printf("ft_isalnum_1_result:%d\n", ft_isalnum('c'));
// 	printf("ft_isalnum_2_result:%d\n", ft_isalnum('!'));
// }
