/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:21:57 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:22:06 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*ptr;
	void	*head;
	size_t	total_size;

	if (count != 0 && SIZE_MAX / count < size)
		return (NULL);
	total_size = count * size;
	ptr = malloc(total_size);
	if (ptr == NULL)
		return (NULL);
	head = ptr;
	while (total_size--)
		*(char *)ptr++ = 0;
	return (head);
}

// bool check_calloc(void)
// {
// }
//
// int main(void)
// {
//   if(check_calloc())
//     printf("OK\n");
//   else
//     printf("ERROR\n");
//}
