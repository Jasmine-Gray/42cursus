/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   compress.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 19:47:16 by mishimod          #+#    #+#             */
/*   Updated: 2024/12/31 19:50:36 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_node	*find_min_unassigned_node(t_stack *stack)
{
	t_node	*current_node;
	t_node	*min_node;

	current_node = stack->top;
	min_node = NULL;
	while (current_node != NULL)
	{
		if (current_node->index == -1 && (min_node == NULL
				|| current_node->value < min_node->value))
		{
			min_node = current_node;
		}
		current_node = current_node->next;
	}
	return (min_node);
}

void	assign_index(t_stack *stack)
{
	int		index;
	t_node	*min_node;

	index = 1;
	while (1)
	{
		min_node = find_min_unassigned_node(stack);
		if (min_node == NULL)
			break ;
		min_node->index = index++;
	}
}
