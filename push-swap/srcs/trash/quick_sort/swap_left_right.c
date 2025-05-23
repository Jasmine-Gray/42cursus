/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap_left_right.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:56:10 by mishimod          #+#    #+#             */
/*   Updated: 2024/12/25 18:28:08 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	close_to_top(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left, t_node **right)
{
	int	count;

	push_b(stack_a, stack_b);
	count = 0;
	if (*right != stack_b->top->next)//add_this_line_but_still_infinite_loop
	{
			while (*right != stack_a->top) // infinite_loop right_pushed_to_stack_b
			{
					reverse_rotate_a(stack_a);
					count++;
			}
			push_b(stack_a, stack_b);
    // if_positions_are_switched_between_left_and_right
	// right_pushed_to_stack_b
	// it_means_right_become_stack_b->top
	}
	swap_b(stack_b);
	retain_value(left, right);
	push_a(stack_a, stack_b);
	while (count != 0)
	{
		rotate_a(stack_a);
		count--;
	}
	push_a(stack_a, stack_b);
}

static void	close_to_bottom(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left, t_node **right)
{
	int	count;

	while (*right == stack_b->top)
	{
		reverse_rotate_a(stack_a);
		push_b(stack_a, stack_b);
	}
	count = 0;
	while (!(*left = stack_a->top))
	{
		reverse_rotate_a(stack_a);
		count++;
	}
	push_b(stack_a, stack_b);
	swap_b(stack_b);
	retain_value(left, right);
	push_a(stack_a, stack_b);
	while (count != 0)
	{
		rotate_a(stack_a);
		count--;
	}
	while (stack_b)
		push_a(stack_a, stack_b);
}

void	swap_left_right(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left, t_node **right)
{
	int	distance;

	distance = measure_distance(stack_a, left);
	if (distance <= 50)
	{
		close_to_top(stack_a, stack_b, left, right);
	}
	if (distance > 50)
	{
		close_to_bottom(stack_a, stack_b, left, right);
	}
}
