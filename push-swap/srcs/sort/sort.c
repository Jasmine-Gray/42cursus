/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/04 21:43:49 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	main_sort(t_linked_list *stack_a, t_linked_list *stack_b, t_node *left,
		t_node *pivot)
{
	t_node	partition;
	t_node	*right;

	partition = *pivot;
	right = pivot->prev;
	if (!stack_a || !left || !right || !pivot)
		return ;
	if (left->next != right->next)
	{
		while (left->value < pivot->value)
			left = left->next;
		if (left->next == right->next)
		{
			swap_left_pivot(stack_a, stack_b, &left, &pivot);
			partition = *left;
			// break ;
		}
		if (left->value > pivot->value)
		{
			while (right->value > pivot->value)
				right = right->prev;
			if (right->value < pivot->value)
			{
				swap_left_right(stack_a, stack_b, &left, &right);
				// break ;
			}
			if (left->prev == right)
			{
				insert_pivot(stack_a, stack_b, &left);
				// break ;
			}
		}
	}
	// if (partition.value != pivot->value)
	// 		main_sort(stack_a, stack_b, left, pivot);
	// if (partition.value != left->value)
	// 		main_sort(stack_a, stack_b, partition.prev, left);
	print_stack(stack_a);
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
	main_sort(stack_a, stack_b, left, pivot);
}
