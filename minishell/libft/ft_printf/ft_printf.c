/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 16:37:32 by mishimod          #+#    #+#             */
/*   Updated: 2025/03/26 17:11:12 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf(const char *format, ...)
{
	int		result;
	int		tmp;
	va_list	ap;

	va_start(ap, format);
	if (!format)
		return (-1);
	result = 0;
	while (*format)
	{
		tmp = 0;
		if (*format == '%')
		{
			tmp = ft_identify_output(format, &ap);
			format++;
		}
		else
			tmp = ft_putchar_fd(*format, 1);
		if (tmp < 0)
			return (-1);
		result += tmp;
		format++;
	}
	va_end(ap);
	return (result);
}

/*
#include <stdio.h>

int	main(void)
{
	char	*str;

	str = "";
//	printf("%d\n", (ft_printf("c=%c\n", 'a')));
//	printf("%d\n", (printf("c=%c\n", 'a')));
//	printf("%d\n", (ft_printf("s=%s\n", "HELLO")));
//	printf("%d\n", (printf("s=%s\n", "HELLO")));
  printf("%d\n", (ft_printf("p=%p\n", "")));
  printf("%d\n", (printf("p=%p\n", "")));
//	printf("%d\n", (ft_printf("d=%d\n", -100)));
//	printf("%d\n", (printf("d=%d\n", -100)));
//	printf("%d\n", (ft_printf("i=%i\n", -100)));
//	printf("%d\n", (printf("i=%i\n", -100)));
//	printf("%d\n", (ft_printf("u=%u\n", -1234567890)));
//	printf("%d\n", (printf("u=%u\n", -1234567890)));
//	printf("%d\n", (ft_printf("x=%x\n", 1234567890)));
//	printf("%d\n", (printf("x=%x\n", 1234567890)));
//	printf("%d\n", (ft_printf("X=%X\n", 1234567890)));
//	printf("%d\n", (printf("X=%X\n", 1234567890)));
//	printf("%d\n", (ft_printf("%%\n")));
//	printf("%d\n", (printf("%%\n")));
	return (0);
}
*/
