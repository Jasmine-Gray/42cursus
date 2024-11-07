/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap_left_pivot.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 18:11:01 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/07 16:43:01 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	close_to_top(t_linked_list *stack_a, t_linked_list *stack_b, t_node **left, t_node **pivot)
{
	print_stack(stack_a);
	while (!(*left == stack_a->top))
	{
		push_b(stack_a, stack_b);
	}
	push_b(stack_a, stack_b);
	reverse_rotate_a(stack_a);
	push_b(stack_a, stack_b);
	swap_b(stack_b);
	retain_pivot(left, pivot);
	push_a(stack_a, stack_b);
	rotate_a(stack_a);
    print_stack(stack_a);
    print_stack(stack_b);
	while (stack_b->top != NULL)
	{
		push_a(stack_a, stack_b);
	}
}

static void	close_to_bottom(t_linked_list *stack_a, t_linked_list *stack_b, t_node **left, t_node **pivot)
{
	int				count;

	reverse_rotate_a(stack_a);
	push_b(stack_a, stack_b);
	count = 0;
	while (*left != stack_b->top)
	{
		reverse_rotate_a(stack_a);
		count++;
	}
	push_b(stack_a, stack_b);
	swap_b(stack_b);
	retain_pivot(left, pivot);
	push_a(stack_a, stack_b);
	while (count != 0)
	{
		rotate_a(stack_a);
		count--;
	}
	push_a(stack_a, stack_b);
	rotate_a(stack_a);
}

void	swap_left_pivot(t_linked_list *stack_a, t_linked_list *stack_b, t_node **left, t_node **pivot)
{
	int				distance;

	distance = measure_distance(stack_a, left);
	if (!stack_a)
		return ;
	if (distance <= 75)
	{
		close_to_top(stack_a, stack_b, left, pivot);
	}
	if (distance > 75)
	{
		close_to_bottom(stack_a, stack_b, left, pivot);
	}
}
