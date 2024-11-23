/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   index_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 13:31:42 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/23 11:52:39 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	find_index_zero(t_linked_list *stack_a)
{
	t_node	*current;

	current = stack_a->top;
	while (one loop)
	{
		if (current->index == 0)
		{
			return (1); // when exist zero
		}
		current = current->next;
	}
	return (0); // after touch index all
}

void	index_stack(t_linked_list *stack_a)
{
	t_node	*current;
	t_node	*last;
	t_node	*min_node;
	int		index;

	last = find_last_node(stack_a);
	min_node = current;
	index = 1;
	while (find_index_zero == 1)
	{
		current = stack_a->top;
		while (1)
		{
			if ((min_node->value > current->value) && (current->index == 0))
			{
				min_node = current;
			}
			if (current == last)
			{
				break ;
			}
			current = current->next;
		}
		min_node->index = index;
		index++;
	}
}
