/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/14 21:28:41 by mishimod         ###   ########.fr       */
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
	while(!is_sort(stack_a))
	{
		while (left->prev != right)
		{
				if (left < pivot)
						left = left->next;
				if (right > pivot)
						right = right->prev;
				if(left > pivot && right < pivot)
				{
						swap_left_right(stack_a, stack_b, left, right);
				}
		}
		if (left == pivot)// sorted
		{
				left = stack_a->top;
				pivot = pivot->prev;
				right = pivot->prev;
		}
		insert_pivot(stack_a, stack_b, left, pivot);// left->prev ni insert
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
	main_sort(stack_a, stack_b, left, pivot);
}
