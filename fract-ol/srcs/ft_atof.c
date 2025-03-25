/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 16:19:33 by mishimod          #+#    #+#             */
/*   Updated: 2025/03/25 18:24:50 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "../fractol.h"

#include "../libft/libft.h"

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

int ft_atof(const char *str)
{
	double result;
	int i;
	int sign;
	double decimal;

	result = 0;
	sign = 1;
	decimal = 1;

	check_sign(str, &i, &sign);
	i = 0;	
	while(str[i] != '\0' && ft_isdigit(str[i]))
	{
		result = result * 10 + str[i] - '0';
		i++;

	}	
	if (str[i] == '.')
		i++;
	while(str[i] != '\0' && ft_isdigit(str[i]))
	{
		result = result * 10 + str[i] - '0';
		decimal *= 0.1;
		i++;
	}
	return ((result * sign) * decimal);
}


int main(int argc, char **argv)
{
	(void)argc;
	ft_atof(*argv);
	
}

















