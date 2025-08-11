/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:36:07 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:36:15 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	skip_space(const char *str)
{
	int	i;

	i = 0;
	while ((str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\v'
			|| str[i] == '\f' || str[i] == '\r'))
		i++;
	return (i);
}

static int	flag_check(const char *str, int *index)
{
	int	flag;

	flag = 1;
	if (str[*(index)] == '-' || str[*(index)] == '+')
	{
		if (str[*(index)] == '-')
			flag = -1;
		(*index)++;
	}
	return (flag);
}

int	ft_atoi(const char *str)
{
	int				i;
	int				size;
	int				flag;
	unsigned long	result;

	result = 0;
	size = ft_strlen(str);
	i = skip_space(str);
	flag = flag_check(str, &i);
	while (i < size && ft_isdigit(str[i]))
	{
		if (flag == -1 && result > ((unsigned long)LONG_MIN - (str[i] - '0'))
			/ 10)
			return ((int)LONG_MIN);
		if (flag == 1 && result > ((unsigned long)LONG_MAX - (str[i] - '0'))
			/ 10)
			return ((int)LONG_MAX);
		result = (result * 10) + (str[i] - '0');
		i++;
	}
	return (flag * result);
}
//
// #include <stdio.h>
// #include <stdlib.h>
//
// int	main(void)
//{
//	char	i_str[16] = "\t\v\f\r\n \f-06050";
//
//	int i_num;
//	int myfunc_num;
//	i_num = atoi(i_str);
//	printf("result atoi:%d\n", i_num);
//	myfunc_num = ft_atoi(i_str);
//	printf("result ft_atoi:%d\n", myfunc_num);
//	return (0);
//}
