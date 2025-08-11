/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:40:36 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:41:48 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 32);
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
