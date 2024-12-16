/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/12/16 17:30:45 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	main_sort(t_linked_list *stack_a, t_linked_list *stack_b)
{
	t_node	*left;
	t_node	*right;
	t_node	*pivot;

	left = NULL;
	pivot = NULL;
	while (is_sort(stack_a) == 0)
	{
		position_pivot(stack_a, &pivot);
		left = stack_a->top;
		right = pivot->prev;
		while (left->prev != right)
		{
			if (left->value < pivot->value)
			{
				left = left->next;
				push_b(stack_a, stack_b);
			}
			if (right->value > pivot->value)
				right = right->prev;
			if ((left->value > pivot->value) && (right->value < pivot->value)
				&& (left->prev != right) && (right != stack_b->top)) // add_condition->infinite_loop
				swap_left_right(stack_a, stack_b, &left, &right);
		}
		insert_pivot(stack_a, stack_b, &left, &pivot);
		while (stack_b->top != NULL)
				push_a(stack_a, stack_b);
		
	}
}

void	sort(t_linked_list *stack_a, t_linked_list *stack_b)
{
	if (!stack_a)
		return ;
	if (!stack_a->top)
		return ;
	main_sort(stack_a, stack_b);
}
