/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy_sort.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/26 16:52:08 by mishimod         ###   ########.fr       */
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
		right = pivot->prev;
		//lldb de check subeki right ha right = stack->top
		while (left->prev != right)
		{
			if (left < pivot)
				left = left->next;
			if (right > pivot)
				right = right->prev;
			if (left > pivot && right < pivot)
			{
				swap_left_right(stack_a, stack_b, &left, &right);
			}
		}
		if (left == pivot)
		{
			left = stack_a->top;
			pivot = pivot->prev;
			right = pivot->prev;
		}
		insert_pivot(stack_a, stack_b, &left, &pivot);
	}
}

void	sort(t_linked_list *stack_a, t_linked_list *stack_b)
{
	t_node	*pivot;
	t_node	*left;

	if (!stack_a)
		return ;
	if (!stack_a->top)
		return ;
	pivot = find_last_node(stack_a);
	left = stack_a->top;
	main_sort(stack_a, stack_b);
}
