/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 21:23:20 by tkusama           #+#    #+#             */
/*   Updated: 2024/04/25 22:34:36 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 32);
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
