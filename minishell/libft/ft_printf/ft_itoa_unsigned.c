/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_unsigned.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:44:54 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/08 15:09:30 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

char	*ft_itoa_unsigned(unsigned int n)
{
	char		*str;
	int			len;
	long long	num;

	num = n;
	len = len_number(n);
	if (n == 0)
		return (ft_strdup("0"));
	str = malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	if (num < 0)
	{
		str[0] = '-';
		num = -num;
	}
	str[len] = '\0';
	while (len-- && num != 0)
	{
		str[len] = num % 10 + '0';
		num /= 10;
	}
	return (str);
}
