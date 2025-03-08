/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_put_fd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 21:26:10 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/23 22:06:22 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

void	ft_puthex_fd(unsigned int i, int fd)
{
	if (i < 16)
		ft_putchar_fd(LOWER[i % 16], fd);
	if (i >= 16)
	{
		ft_puthex_fd(i / 16, fd);
		ft_putchar_fd(LOWER[i % 16], fd);
	}
}

void	ft_put_caphex_fd(unsigned int i, int fd)
{
	if (i < 16)
		ft_putchar_fd(UPPER[i % 16], fd);
	if (i >= 16)
	{
		ft_put_caphex_fd(i / 16, fd);
		ft_putchar_fd(UPPER[i % 16], fd);
	}
}

void	ft_putptr_fd(unsigned long long i, int fd)
{
	if (i < 16)
		ft_putchar_fd(LOWER[i % 16], fd);
	if (i >= 16)
	{
		ft_putptr_fd(i / 16, 1);
		ft_putchar_fd(LOWER[i % 16], fd);
	}
}

void	ft_putunit_fd(unsigned int i, int fd)
{
	if (i < 10)
		ft_putchar_fd(i % 10 + '0', fd);
	if (i >= 10)
	{
		ft_putunit_fd(i / 10, fd);
		ft_putchar_fd(i % 10 + '0', fd);
	}
}
