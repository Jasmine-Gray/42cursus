/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:27:41 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/17 18:38:45 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	if (n < 0)
	{
		write(fd, "-", 1);
		if (n != INT_MIN)
		{
			n *= -1;
		}
	}
	if (n == INT_MIN)
	{
		ft_putnbr_fd((n / 10) * -1, fd);
	}
	else if (n >= 10)
		ft_putnbr_fd(n / 10, fd);
	if (n < 0)
		ft_putchar_fd((n % 10) * -1 + '0', fd);
	else
		ft_putchar_fd((n % 10) + '0', fd);
}

// int main(void) {
// 	ft_putnbr_fd(0, 1);
// 	printf("\n");
// 	ft_putnbr_fd(-0, 1);
// 	printf("\n");
// 	ft_putnbr_fd(+0, 1);
// 	printf("\n");
// 	ft_putnbr_fd(1, 1);
// 	printf("\n");
// 	ft_putnbr_fd(-1, 1);
// 	printf("\n");
// 	ft_putnbr_fd(10, 1);
// 	printf("\n");
// 	ft_putnbr_fd(-10, 1);
// 	printf("\n");
// 	ft_putnbr_fd(100, 1);
// 	printf("\n");
// 	ft_putnbr_fd(-100, 1);
// 	printf("\n");
// 	ft_putnbr_fd(INT_MAX, 1);
// 	printf("\n");
// 	ft_putnbr_fd(INT_MIN, 1);
// 	printf("\n");
// }
