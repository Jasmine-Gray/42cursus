/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 16:34:02 by mishimod          #+#    #+#             */
/*   Updated: 2024/04/16 17:12:04 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isascii(int c)
{
	if (0 <= c && c <= 0177)
	{
		return (1);
	}
	return (0);
}

// bool	check_isascii(void)
// {
// 	int	i;

// 	i = 0;
// 	while (i < 1024)
// 	{
// 		if (ft_isascii(i) != isascii(i))
// 		{
// 			printf("ft_isascii=%d, isascii=%d\n", ft_isascii(i), isascii(i));
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }

// int	main(void)
// {
// 	if (check_isascii())
// 	{
// 		printf("OK\n");
// 	}
// 	else
// 	{
// 		printf("ERROR\n");
// 	}
// }
