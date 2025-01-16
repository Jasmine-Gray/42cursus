/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:20:51 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/16 13:32:12 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	ft_isspace(unsigned char s)
{
	if ((s >= 9 && s <= 13) || s == 32)
		return (1);
	return (0);
}

static void	check_sign(const char *str, size_t *i, int *sign)
{
	while (ft_isspace(str[*i]))
		(*i)++;
	if (str[*i] == '-' || str[*i] == '+')
	{
		if (str[*i] == '-')
			*sign = -1;
		(*i)++;
	}
}

int	ft_atoi(const char *str)
{
	unsigned long	result;
	int				sign;
	size_t			i;

	result = 0;
	sign = 1;
	i = 0;
	check_sign(str, &i, &sign);
	while (str[i] >= '0' && str[i] <= '9' && str[i] != '\0')
	{
		if (sign == -1 && result > ((unsigned long)LONG_MAX + 1 - (str[i]
					- '0')) / 10)
			return ((int)LONG_MIN);
		if (sign != -1 && result > ((unsigned long)LONG_MAX - (str[i] - '0'))
			/ 10)
			return ((int)LONG_MAX);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

// bool check_atoi(void)
// {
// }
//
// int main(void)
// {
//   if(check_atoi())
//     printf("OK\n");
//   else
//     printf("OK\n");
// }
//
