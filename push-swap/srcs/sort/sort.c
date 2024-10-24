/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/24 21:32:45 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void sort(t_linked_list *stack)
{
	t_node	*pivot;
	t_node	*left;
	t_node	*right;

	pivot = find_last_node(stack);
	left = stack->top;
	right = pivot->prev;

	while (!(left->next == right->next))
	{
		while (left->value < pivot->value)
				left = left->next;
		if (left->next == right->next)
		{
				swap_left_pivot(stack, left, pivot);
				break ;
		}
		if (left->value > pivot->value) 
		{
				while (right->value > pivot->value)
						right = right->prev;
				if (right->value < pivot->value)
						swap_left_right(stack, left, right);
				if (left->prev == right->next)
				{
						//insert pivot to between L and R
				}
		}
	}
}
