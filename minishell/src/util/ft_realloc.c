/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 20:53:38 by mishimod          #+#    #+#             */
/*   Updated: 2025/08/10 04:06:50 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	*ft_malloc(size_t size)
{
	void	*block_start;
	void	*user_ptr;

	block_start = malloc(sizeof(size_t) + size);
	if (block_start == NULL)
		return (NULL);
	*(size_t *)block_start = size;
	user_ptr = (char *)block_start + sizeof(size_t);
	return (user_ptr);
}

static void	ft_free(void *ptr)
{
	void	*block_start;

	if (ptr == NULL)
		return ;
	block_start = (char *)ptr - sizeof(size_t);
	free(block_start);
}

void	*ft_realloc(void *ptr, size_t new_size)
{
	void	*new_ptr;
	void	*block_start;
	size_t	old_size;
	size_t	copy_size;

	if (ptr == NULL)
		return (ft_malloc(new_size));
	if (new_size == 0)
	{
		ft_free(ptr);
		return (NULL);
	}
	block_start = (char *)ptr - sizeof(size_t);
	old_size = *(size_t *)block_start;
	new_ptr = ft_malloc(new_size);
	if (new_ptr == NULL)
		return (NULL);
	if (old_size < new_size)
		copy_size = old_size;
	else
		copy_size = new_size;
	ft_memcpy(new_ptr, ptr, copy_size);
	ft_free(ptr);
	return (new_ptr);
}
