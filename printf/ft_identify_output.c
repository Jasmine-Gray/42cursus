/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_identify_output.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 13:13:16 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/23 21:13:01 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

int	ft_identify_output(const char *str, va_list *ap)
{
	str++;
	if (*str == 'c')
		return (ft_print_char(ap));
	if (*str == 's')
		return (ft_print_str(ap));
	if (*str == 'p')
		return (ft_print_ptr(ap));
	if (*str == 'd')
		return (ft_print_int(ap));
	if (*str == 'i')
		return (ft_print_int(ap));
	if (*str == 'u')
		return (ft_print_unit(ap));
	if (*str == 'x')
		return (ft_print_hex(ap));
	if (*str == 'X')
		return (ft_print_caphex(ap));
	if (*str == '%')
		return (ft_print_percent());
	return (-1);
}
