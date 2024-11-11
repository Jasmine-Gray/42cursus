/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/11 17:00:07 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	main_sort(t_linked_list *stack_a, t_linked_list *stack_b, t_node *left,
		t_node *pivot)
{
	t_node	*partition;
	t_node	*right;

    partition = NULL;
	right = pivot->prev;
	if (!stack_a || !left || !right || !pivot)
		return ;
	while (left->next != right->next)
	{
		print_stack(stack_a);
		while (left->value < pivot->value)
			left = left->next;
		if (left->next == right->next)
		{
		    print_stack(stack_a);
			swap_left_pivot(stack_a, stack_b, &left, &pivot);
			if (partition == NULL)
		    	partition = left->prev;
			if (left->next != pivot)
				main_sort(stack_a, stack_b, left, pivot);
			print_stack(stack_a);
		}
		if (left->value > pivot->value)
		{
			while (right->value > pivot->value)
				right = right->prev;
			if (right->value < pivot->value)
			{
				swap_left_right(stack_a, stack_b, &left, &right);
			}
			if (left->prev == right)
			{
				insert_pivot(stack_a, stack_b, &left, &pivot);
				if (left->next != pivot)
				{
						if (partition == NULL)
								partition = left->prev;
						main_sort(stack_a, stack_b, left, pivot);
				}
			}
		}
	}
	if (partition != NULL)
	  		main_sort(stack_a, stack_b, left, partition);
	//  if (partition.value == left->value)
	//  		main_sort(stack_a, stack_b, left, pivot);
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
