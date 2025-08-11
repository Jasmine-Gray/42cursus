/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:40:01 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:41:22 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}

// #include <assert.h>
// #include <stdio.h>

// void	test_ft_strlen(void)
// {
// 	printf("result:%d\n", ft_strlen("Hello, world!") == 13);
// 	printf("result:%d\n", ft_strlen("") == 0
// 		&& "Length of an empty string should be 0.");
// 	printf("result:%d\n", ft_strlen("a") == 1 && "Length of 'a' should be 1.");
// 	printf("result:%d\n",
// 	ft_strlen("This is a very long string used specifically for ") == 49);
// }

// int	main(void)
// {
// 	test_ft_strlen();
// 	return (0);
// }
