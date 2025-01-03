/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 19:40:27 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/04 07:41:08 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	radix_sort(t_linked_list *stack_a, t_linked_list *stack_b)
{
	size_t	stack_size;
	size_t	len;

	stack_size = count_node(stack_a);
	len = 0;
	while ()
	{
		if (is_sort(stack_a))
			break ;
		while (len < stack_size)
		{
			if (((>>) & 1) == 0) 
				push_b(stack_a, stack_b);
			else
				rotate_a(stack_a);
			len++;
		}
		while (stack_b)
			push_a(stack_a, stack_b);
	}
}
