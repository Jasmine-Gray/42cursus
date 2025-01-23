/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_xpercent.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 18:43:41 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/21 21:58:16 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

static int	ft_hex_digit(unsigned int i)
{
	int	count;

	count = 0;
	while (i >= 16)
	{
		i /= 16;
		count++;
	}
	count++;
	return (count);
}

int	ft_print_hex(va_list *ap)
{
	unsigned int	i;

	i = va_arg(*ap, unsigned int);
	ft_puthex_fd(i, 1);
	return (ft_hex_digit(i));
}

int	ft_print_caphex(va_list *ap)
{
	unsigned int	i;

	i = va_arg(*ap, unsigned int);
	ft_put_caphex_fd(i, 1);
	return (ft_hex_digit(i));
}

int	ft_print_percent(void)
{
	return (ft_putchar_fd('%', 1));
}
