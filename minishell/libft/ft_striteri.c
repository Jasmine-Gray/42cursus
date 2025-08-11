/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:39:51 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:39:52 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	unsigned int	i;

	i = 0;
	if (s && f)
	{
		while (s[i] != '\0')
		{
			f(i, &s[i]);
			i++;
		}
	}
}

// void	lowercase(unsigned int i, char *c)
//{
//   (void)i;
//	*c = ft_tolower(*c);
// }
//
// #include <stdio.h>
//
// int	main(void)
//{
//	char	str[] = "hElLo WoRlD";
//
//	ft_striteri(str, lowercase);
//	printf("Modified string: %s\n", str);
//	return (0);
// }
