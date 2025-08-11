/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:40:41 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:40:54 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}

// #include <ctype.h>
// #include <stdio.h>
//
//  int	main(void)
//{
//	printf("%c\n", toupper('a'));
//	printf("%c\n", toupper('R'));
//	printf("%c\n", ft_toupper('a'));
//	printf("%c\n", ft_toupper('R'));
//	return (0);
// }
