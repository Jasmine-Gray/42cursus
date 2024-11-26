/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   index_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 13:31:42 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/26 20:12:51 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	find_index_zero(t_linked_list *stack_a)
{
	t_node	*current;
	t_node	*last;

	current = stack_a->top;
	last = find_last_node(stack_a);
	while (1)
	{
		if (current->index == 0)
		{
			return (1);
		}
		if (current == last)
		{
			break ;
		}
		current = current->next;
	}
	return (0);
}

void	index_stack(t_linked_list *stack_a)
{
	t_node	*current;
	t_node	*last;
	t_node	*min_node;
	int		index;

	last = find_last_node(stack_a);
	index = 1;
	while (find_index_zero(stack_a) == 1)
	{
		current = stack_a->top;
		while (1)
		{
			if (current->index == 0)
				break ;
			current = current->next;
		}
		min_node = current;
		while (1)
		{
			if ((min_node->value > current->value) && (current->index == 0))
				min_node = current;
			if (current == last)
				break ;
			current = current->next;
		}
		min_node->index = index;
		index++;
	}
}
