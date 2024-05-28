/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:23:01 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:52:57 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_digit(int n)
{
	int	digit;

	if (n == 0)
	{
		return (1);
	}
	digit = 0;
	while (n != 0)
	{
		n /= 10;
		digit++;
	}
	return (digit);
}

char	*ft_itoa(int n)
{
	char	*result;
	int		digit;

	digit = count_digit(n);
	if (n < 0)
	{
		digit += 1;
	}
	if (n == 0)
		return (ft_strdup("0"));
	result = malloc(sizeof(char) * (digit + 1));
	if (!result)
		return (NULL);
	if (n < 0)
		result[0] = '-';
	result[digit] = '\0';
	while (n != 0)
	{
		if (n < 0)
			result[--digit] = (n % 10) * -1 + '0';
		else
			result[--digit] = (n % 10) * 1 + '0';
		n /= 10;
	}
	return (result);
}

// __attribute__((destructor))
// static void destructor() {
//     system("leaks -q a.out");
// }

// int	main(void)
// {
//	printf("%s\n", ft_itoa(0));
//	printf("%s\n", ft_itoa(-0));
//	printf("%s\n", ft_itoa(+0));
//	printf("%s\n", ft_itoa(1));
//	printf("%s\n", ft_itoa(-1));
//	printf("%s\n", ft_itoa(10));
//	printf("%s\n", ft_itoa(-10));
//	printf("%s\n", ft_itoa(INT_MAX));
//	printf("%s\n", ft_itoa(INT_MIN));
// 	return (0);
// }
