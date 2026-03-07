/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 21:23:20 by tkusama           #+#    #+#             */
/*   Updated: 2024/04/23 22:19:18 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}

//#include <ctype.h>
//#include <stdio.h>
//
// int	main(void)
//{
//	printf("%c\n", toupper('a'));
//	printf("%c\n", toupper('R'));
//	printf("%c\n", ft_toupper('a'));
//	printf("%c\n", ft_toupper('R'));
//	return (0);
//}
