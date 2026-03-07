/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 20:16:18 by tkusama           #+#    #+#             */
/*   Updated: 2024/05/10 20:09:17 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isdigit(int c)
{
	if ((c >= '0' && c <= '9'))
		return (1);
	return (0);
}

// #include <stdio.h>
// #include <ctype.h>
// int	main(void)
// {
// 	printf("isdigit_1:result:%d\n", isdigit('c'));
// 	printf("isdigit_2:result:%d\n", isdigit('!'));
// 	printf("ft_isdigit_1:result:%d\n", ft_isdigit('c'));
// 	printf("ft_isdigit_2:result:%d\n", ft_isdigit('!'));
// }
