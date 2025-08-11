/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:36:29 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:36:31 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*ptr;
	char	*p;
	size_t	i;
	size_t	memory_size;

	i = 0;
	memory_size = count * size;
	if (count != 0 && (SIZE_MAX / count) < size)
		return (NULL);
	ptr = malloc(memory_size);
	if (ptr == NULL)
		return (NULL);
	p = ptr;
	while (i < memory_size)
	{
		p[i] = 0;
		i++;
	}
	return (ptr);
}

// int	main(void)
//{
//	size_t	*ptr;
//
//	ptr = ft_calloc(1, 1);
//	return (0);
//}
