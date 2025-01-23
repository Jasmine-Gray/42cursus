/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_diu.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 18:38:43 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/23 21:19:16 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

int	ft_print_int(va_list *ap)
{
	int	i;

	i = va_arg(*ap, int);
	ft_putnbr_fd(i, 1);
	return (ft_digit(i));
}

int	ft_print_unit(va_list *ap)
{
	unsigned int	i;

	i = va_arg(*ap, unsigned int);
	ft_putunit_fd(i, 1);
	return (ft_digit(i));
}
