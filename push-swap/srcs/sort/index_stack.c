/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   index_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 13:31:42 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/22 16:49:14 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	index_stack(t_linked_list *stack_a)
{
	t_node	*current;
	t_node	*last;
	t_node	*min;
	int		index;

	current = stack_a->top;
	last = find_last_node(stack_a);
	min = current;
	index = 0;

	while (current != last)
	{
		if (min->value > current->next->value)
		{
			min = current->next;
			index = min->value;
		}
		current = current->next;
	}
}
