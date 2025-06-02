/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:27:18 by mishimod          #+#    #+#             */
/*   Updated: 2025/03/08 19:49:20 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
	return (0);
}

// int main(void)
// {
//   char c = 'a';
//   ft_putchar_fd(c, 1);
//   return (0);
// }
