/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap_left_right.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:56:10 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/24 21:34:45 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"


static void	close_to_top(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node *left, t_node *right)
{
	int	count;

	while (!(left->next == stack_a->top))
	{
		push_b(stack_a, stack_b);
	}
	count = 0;
	while (!(right = stack_a->top))
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
}

// static void	close_to_bottom(t_linked_list *stack_a, t_linked_list *stack_b,
// 		t_node *left, t_node *right)
// {
// 	int	count;
// 
// 	while (right == stack_b->top)
// 	{
// 		reverse_rotate_a(stack_a);
// 		push_b(stack_a, stack_b);
// 	}
// 	count = 0;
// 	while (!(left = stack_a->top))
// 	{
// 		reverse_rotate_a(stack_a);
// 		count++;
// 	}
// 	push_b(stack_a, stack_b);
// 	swap_b(stack_b);
// 	retain_value(left, right);
// 	push_a(stack_a, stack_b);
// 	while (count != 0)
// 	{
// 		rotate_a(stack_a);
// 		count--;
// 	}
// 	while (stack_b)
// 		push_a(stack_a, stack_b);
// }

void	swap_left_right(t_linked_list *stack, t_node *left, t_node *right)
{
	t_linked_list	*stack_a;
	t_linked_list	*stack_b;
	//int				distance;

	stack_a = stack;
	stack_b = NULL;
	//distance = measure_distance();
	// if (distance == close from top)
	// {
	close_to_top(stack_a, stack_b, left, right);
	// }
	// if (distance == close from bottom)
	// {
	// 	close_to_bottom(stack_a, stack_b, left, right);
	// }
}

