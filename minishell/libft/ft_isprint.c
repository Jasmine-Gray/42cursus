/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:12:32 by mishimod          #+#    #+#             */
/*   Updated: 2024/04/16 19:08:29 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isprint(int c)
{
	if (c >= 040 && c <= 0176)
	{
		return (1);
	}
	return (0);
}

// bool	check_isprint(void)
// {
// 	int	i;

// 	i = 0;
// 	while (i < 1024)
// 	{
// 		if (ft_isprint(i) != isprint(i))
// 		{
// 			printf("ft_isprint=%d, isprint=%d\n", ft_isprint(i), isprint(i));
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }

// int	main(void)
// {
// 	if (check_isprint())
// 	{
// 		printf("ok\n");
// 	}
// 	else
// 	{
// 		printf("error\n");
// 	}
// }
