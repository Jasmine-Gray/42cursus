/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/12/29 19:17:51 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	main_sort(t_linked_list *stack_a, t_linked_list *stack_b)
{
	t_node	*left;
	t_node	*right;
	t_node	*pivot;
	t_node	*flag;

	left = NULL;
	pivot = NULL;
	flag = NULL;
	while (is_sort(stack_a) == 0)
	{
		position_pivot(stack_a, stack_b, &pivot);
		left = stack_a->top;
		right = pivot->prev;
		set_flag(stack_b, &flag);
		while ((left->prev != right) || (stack_b->top->value != right->value))
		{
			if (left->value < pivot->value)
			{
				left = left->next;
				push_b(stack_a, stack_b);
			}
			if (right->value > pivot->value)
			{
					if (stack_a->top == right)
							right = stack_b->top;
					else
							right = right->prev;
			}
			if ((left->value > pivot->value) && (right->value < pivot->value)
				&& (left->prev != right) && (stack_b->top != right))
				swap_left_right(stack_a, stack_b, &left, &right);
		}
		insert_pivot(stack_a, stack_b, &left, &pivot);
		while ((stack_b->top != NULL) && (stack_b->top != flag))
			push_a(stack_a, stack_b);
		if ((is_sort(stack_a) == 1) && (is_sort(stack_b) == 1))
		{
			while (stack_b->top != NULL)
				push_a(stack_a, stack_b);
		}
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
