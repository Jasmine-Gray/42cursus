/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:20:51 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/16 12:12:00 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	ft_isspace(unsigned char s)
{
	return ((s >= 9 && s <= 13) || s == 32);
	// 	return (1);
	// return (NULL);
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

int	*ft_atoi(const char *str)
{
	unsigned long	result;
	int				sign;
	size_t			i;

	result = 0;
	sign = 1;
	i = 0;
	if (!str)
		return (NULL);
	check_sign(str, &i, &sign);
	while (str[i] >= '0' && str[i] <= '9' && str[i] != '\0')
	{
		if (sign == -1 && result > ((unsigned long)LONG_MAX + 1 - (str[i]
					- '0')) / 10)
			return (NULL);
		if (sign != -1 && result > ((unsigned long)LONG_MAX - (str[i] - '0'))
			/ 10)
			return (NULL);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	if (str[i] != '\0')
		return (NULL);
	return ((int *)(result * sign));
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
