/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 14:48:49 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/09 20:36:55 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	return (ft_isdigit(c) || ft_isalpha(c));
}

// bool	check_isalnum(void)
// {
// 	int	i;

// 	i = 0;
// 	while (i < 1048)
// 	{
// 		if (isalnum(i) != ft_isalnum(i))
// 		{
// 			printf("%d, isalnum=%d, ft_isalnum=%d\n", i, isalnum(i),
// 				ft_isalnum(i));
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }

// int	main(void)
// {
// 	if (check_isalnum())
// 	{
// 		printf("OK\n");
// 	}
// 	else
// 	{
// 		printf("ERROR\n");
// 	}
// }
