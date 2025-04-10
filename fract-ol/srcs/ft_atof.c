/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 16:19:33 by mishimod          #+#    #+#             */
/*   Updated: 2025/04/10 19:24:15 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

static char	ft_isspace(unsigned char s)
{
	if ((s >= 9 && s <= 13) || s == 32)
		return (1);
	return (0);
}

static void	check_sign(const char *str, size_t *i, double *sign)
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

double	ft_atof(const char *str)
{
	double	result;
	size_t	i;
	double	sign;
	double	decimal;

	result = 0.0;
	i = 0;
	sign = 1.0;
	decimal = 1.0;
	check_sign(str, &i, &sign);
	while (str[i] != '\0' && ft_isdigit(str[i]))
	{
		result = result * 10 + str[i] - '0';
		i++;
	}
	if (str[i] == '.')
		i++;
	while (str[i] != '\0' && ft_isdigit(str[i]))
	{
		result = result * 10 + str[i] - '0';
		decimal *= 0.1;
		i++;
	}
	return (result * sign * decimal);
}

// int	main(int argc, char **argv)
// {
// 	double	result;
//
// 	(void)argc;
// 	result = ft_atof(argv[1]);
// 	printf("result=%f\n", result);
// 	return (0);
// }
