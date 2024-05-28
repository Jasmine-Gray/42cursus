/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 23:46:42 by mishimod          #+#    #+#             */
/*   Updated: 2024/04/18 21:37:11 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_isupper(int c)
{
	if ('A' <= c && c <= 'Z')
		return (1);
	return (0);
}

static int	ft_islower(int c)
{
	if ('a' <= c && c <= 'z')
		return (1);
	return (0);
}

int	ft_isalpha(int c)
{
	return (ft_isupper(c) || ft_islower(c));
}

// bool check_isalpha(void)
// {
// 	int i = 0;
//
// 	while (i < 1024)
// 	{
// 		if (isalpha(i) != ft_isalpha(i))
// 		{
// 			printf("%d\t isalpha = %d, ft_isalpha = %d\n", i, isalpha(i),
//          ft_isalpha(i));
// 			return (false);
// 		}
// 		i++;
// 	}
// 	return (true);
// }
//
// int main(void)
// {
//
// 	if (check_isalpha())
// 	{
// 		printf("ok\n");
// 	} else
// 	{
// 		printf("error\n");
// 	}
// }
