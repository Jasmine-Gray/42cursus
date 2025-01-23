/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_csp.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 18:35:31 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/23 22:10:34 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

int	ft_print_char(va_list *ap)
{
	char	c;

	c = va_arg(*ap, int);
	return (ft_putchar_fd(c, 1));
}

int	ft_print_str(va_list *ap)
{
	char	*str;

	str = va_arg(*ap, char *);
	if (!str)
		return (ft_putstr_fd("(null)", 1));
	return (ft_putstr_fd(str, 1));
}

static int	ft_ptr_digit(unsigned long long i)
{
	int	count;

	count = 0;
	while (i >= 16)
	{
		i /= 16;
		count++;
	}
	return (count);
}

int	ft_print_ptr(va_list *ap)
{
	unsigned long long	ptr;

	ptr = va_arg(*ap, unsigned long long);
	write(1, "0x", 2);
	ft_putptr_fd(ptr, 1);
	return (ft_ptr_digit(ptr) + ft_strlen("0×"));
}
