/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtol.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 10:18:01 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/18 22:38:55 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

long	ft_strtol(const char *nptr, char **endptr)
{
	const char	*s = nptr;
	long		result;
	int			sign;

	result = 0;
	sign = 1;
	if (*s == '-')
	{
		sign = -1;
		s++;
	}
	else if (*s == '+')
		s++;
	while (*s >= '0' && *s <= '9')
	{
		result = result * 10 + (*s - '0');
		if ((result > INT_MAX && sign == 1)
			|| (-result < INT_MIN && sign == -1))
			break ;
		s++;
	}
	if (endptr != NULL)
		*endptr = (char *)s;
	return (sign * result);
}
