/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 16:29:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/04 17:46:40 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	main_sort(t_linked_list *stack_a, t_linked_list *stack_b, t_node *left, t_node *pivot)
{
	t_node	partition;
	t_node	*right;

	partition = *pivot;
	right = pivot->prev;

	if (!stack_a || !left || !right || !pivot)
		return ;
	printf("1left=%d\n", left->value);
	printf("1right=%d\n", right->value);
	printf("1pivot=%d\n", pivot->value);
	if (!(left->next == right->next))
	{
			while (left->value < pivot->value)
					left = left->next;
			printf("2left=%d\n", left->value);
			printf("2right=%d\n", right->value);
			printf("2pivot=%d\n", pivot->value);
			if (left->next == right->next)
			{
					swap_left_pivot(stack_a, stack_b, left, pivot);
					partition = *left;
					//break ;
			}
			if (left->value > pivot->value)
			{
					while (right->value > pivot->value)
							right = right->prev;
					if (right->value < pivot->value)
					{
							swap_left_right(stack_a, stack_b, left, right);
							//break ;
					}
					if (left->prev == right)
					{
							insert_pivot(stack_a, left);
							//break ;
					}
			}
	}
	printf("partition=%d\n", partition.value);
	if (partition.value != pivot->value)
			main_sort(stack_a, stack_b, left, pivot);
	if (partition.value != left->value)
			main_sort(stack_a, stack_b, partition.prev, left);
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

// void	quick_sort_recursive(t_linked_list *stack, t_node *left, t_node *pivot)
// {
// 	t_node	*right;
// 	t_node *partition;
//
// 	if (!left || !pivot || left == pivot || left->next == pivot)
// 		return ;
//
// 	partition = NULL;
// 	right = pivot->prev;
// 	while (left != right && left->prev != right)
// 	{
// 		while (left != right && left->value < pivot->value)
// 			left = left->next;
// 		while (right != left && right->value > pivot->value)
// 			right = right->prev;
// 		if (left != right)
// 		{
// 			if (left->value > pivot->value && right->value < pivot->value)
// 				swap_left_right(stack, left, right);
// 		}
// 	}
// 	if (left->value > pivot->value)
// 	{
// 		swap_left_pivot(stack, left, pivot);
// 		partition = left;
// 	}
// 	else
// 	{
// 		partition = pivot;
// 	}
//
// 	if (left != partition)
// 			quick_sort_recursive(stack, left, partition.prev);
// 	if (partition != pivot)
// 			quick_sort_recursive(stack, partition.next, pivot);
// }
