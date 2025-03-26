/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 17:03:38 by mishimod          #+#    #+#             */
/*   Updated: 2025/03/26 17:10:07 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "../libft.h"
# include <stdarg.h>
# include <stdio.h>

# define LOWER "0123456789abcdef"
# define UPPER "0123456789ABCDEF"

int		ft_printf(const char *format, ...);
int		ft_identify_output(const char *str, va_list *app);
int		ft_digit(long long i);
int		ft_print_char(va_list *ap);
int		ft_print_str(va_list *ap);
int		ft_print_ptr(va_list *ap);
int		ft_print_int(va_list *ap);
int		ft_print_hex(va_list *ap);
int		ft_print_unit(va_list *ap);
int		ft_print_caphex(va_list *ap);
int		ft_print_percent(void);
void	ft_puthex_fd(unsigned int i, int fd);
void	ft_put_caphex_fd(unsigned int i, int fd);
void	ft_putunit_fd(unsigned int i, int fd);
void	ft_putptr_fd(unsigned long long i, int fd);

#endif
