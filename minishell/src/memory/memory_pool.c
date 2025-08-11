/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_pool.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 11:45:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:55:54 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	memory_pool_init(t_memory_pool *pool)
{
	pool->ptrs = NULL;
	pool->count = 0;
	pool->capacity = 0;
}

static int	memory_pool_expand(t_memory_pool *pool)
{
	void	**new_ptrs;
	size_t	new_capacity;

	if (pool->count < pool->capacity)
		return (0);
	if (pool->capacity == 0)
		new_capacity = 16;
	else
		new_capacity = pool->capacity * 2;
	new_ptrs = malloc(sizeof(void *) * new_capacity);
	if (!new_ptrs)
		return (-1);
	if (pool->ptrs)
	{
		ft_memcpy(new_ptrs, pool->ptrs, sizeof(void *) * pool->count);
		free(pool->ptrs);
	}
	pool->ptrs = new_ptrs;
	pool->capacity = new_capacity;
	return (0);
}

int	memory_pool_add(t_memory_pool *pool, void *ptr)
{
	if (memory_pool_expand(pool) != 0)
		return (-1);
	pool->ptrs[pool->count] = ptr;
	pool->count++;
	return (0);
}

void	memory_pool_remove(t_memory_pool *pool, void *ptr)
{
	size_t	i;

	i = 0;
	while (i < pool->count)
	{
		if (pool->ptrs[i] == ptr)
		{
			pool->ptrs[i] = pool->ptrs[--pool->count];
			break ;
		}
		i++;
	}
}

void	memory_pool_cleanup(t_memory_pool *pool)
{
	size_t	i;

	i = 0;
	while (i < pool->count)
	{
		free(pool->ptrs[i]);
		i++;
	}
	free(pool->ptrs);
	pool->ptrs = NULL;
	pool->count = 0;
	pool->capacity = 0;
}
