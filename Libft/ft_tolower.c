/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:30:53 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/09 14:27:46 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + ('a' - 'A'));
	return (c);
}

// bool	check_tolower(void)
// {
// 	unsigned char	str_1[256];
// 	unsigned char	str_2[256];
// 	int				i;
//
// 	i = 0;
// 	while (i < 256)
// 	{
// 		str_1[i] = '\0';
// 		str_2[i] = '\0';
// 		i++;
// 	}
// 	if (ft_tolower(str_1[i]) != tolower(str_2[i]))
// 	{
// 		printf("ft_tolower=%c, tolower=%c\n", str_1[i], str_2[i]);
// 		return (false);
// 	}
// 	return (true);
// }
//
// int	main(void)
// {
// 	if (check_tolower())
// 	{
// 		printf("OK\n");
// 	}
// 	else
// 	{
// 		printf("ERROR\n");
// 	}
// }
