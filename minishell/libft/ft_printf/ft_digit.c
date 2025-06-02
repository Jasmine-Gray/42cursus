/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_digit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 19:00:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/03/26 17:10:14 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_digit(long long i)
{
	int	count;

	if (i == 0)
		return (1);
	count = 0;
	if (i < 0)
	{
		count++;
		i *= -1;
	}
	while (i > 0)
	{
		i /= 10;
		count++;
	}
	return (count);
}
