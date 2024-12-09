/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_sort.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/12/09 16:32:07 by mishimod         ###   ########.fr       */
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
	if (!stack_a)
		return ;
	while (is_sort(stack_a) == 0)
	{
		position_pivot(stack_a, &pivot);
		left = stack_a->top;
		right = pivot->prev;
		while (left->prev != right)
		{
			if (left->value < pivot->value)
				left = left->next;
			if (right->value > pivot->value)
				right = right->prev;
			if ((left->value > pivot->value) && (right->value < pivot->value)
				&& (left->prev != right))
			{
				swap_left_right(stack_a, stack_b, &left, &right);
				print_stack(stack_a);
			}
		}
		// if ((left == right) || (left->prev == right))
		insert_pivot(stack_a, stack_b, &left, &pivot);
		print_stack(stack_a);
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
