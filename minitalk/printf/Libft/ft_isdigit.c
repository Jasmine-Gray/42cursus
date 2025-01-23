/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 13:57:47 by mishimod          #+#    #+#             */
/*   Updated: 2024/04/22 21:10:03 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

// bool	check_isdigit(void)
// {
// 	int	i;

// 	i = 0;
// 	while (i < 1024)
// 	{
// 		if (isdigit(i) != ft_isdigit(i))
// 		{
// 			printf("%d\t, isdigit=%d, ft_isdigit=%d\n", i, isdigit(i),
// 				ft_isdigit(i));
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }

// int main(void)
// {
//
// 	if (check_isdigit())
// 	{
// 		printf("ok\n");
// 	} else
// 	{
// 		printf("error\n");
// 	}
// }
