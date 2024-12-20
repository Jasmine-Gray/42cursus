/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   position_pivot.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 14:54:28 by mishimod          #+#    #+#             */
/*   Updated: 2024/12/21 02:32:07 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	position_pivot(t_linked_list *stack_a, t_linked_list *stack_b, t_node **pivot)
{
	int	stack_size;

	stack_size = count_node(stack_a);
	(*pivot) = stack_a->top->prev;
	while (stack_size)
	{
		if (stack_size != (*pivot)->index)
		{
			break ;
		}
		reverse_rotate_a(stack_a);
		push_b(stack_a, stack_b);
		(*pivot) = (*pivot)->prev;
		stack_size--;
	}
}
