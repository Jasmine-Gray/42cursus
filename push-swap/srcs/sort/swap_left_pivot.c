/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap_left_pivot.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 18:11:01 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/24 21:34:28 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"


static void	close_to_top(t_linked_list *stack, t_node *left, t_node *pivot)
{
	t_linked_list	*stack_a;
	t_linked_list	*stack_b;

	stack_a = stack;
	stack_b = NULL;
	while (!(left->next == stack_a->top))
	{
		push_b(stack_a, stack_b);
	}
	reverse_rotate_a(stack_a);
	push_b(stack_a, stack_b);
	swap_b(stack_b);
	retain_pivot(left, pivot);
	push_a(stack_a, stack_b);
	rotate_a(stack_a);
	while (!(stack_b = NULL))
		push_a(stack_a, stack_b);
}

//  static void	close_to_bottom(t_linked_list *stack, t_node *left, t_node *pivot)
//  {
//      t_linked_list *stack_a;
//      t_linked_list *stack_b;
//  	int	count;
//  	stack_a = stack;
//  	stack_b = NULL;
//  
//  	reverse_rotate_a(stack_a);
//  	push_b(stack_a, stack_b);
//  	count = 0;
//  	while (left != stack_b->top)
//  	{
//  		reverse_rotate_a(stack_a);
//  		count++;
//  	}
//  	push_b(stack_a, stack_b);
//  	swap_b(stack_b);
//  	retain_pivot(left, pivot);
//  	push_a(stack_a, stack_b);
//  	while (count != 0)
//  	{
//  		rotate_a(stack_a);
//  		count--;
//  	}
//  	push_a(stack_a, stack_b);
//  	rotate_a(stack_a);
//  }

void	swap_left_pivot(t_linked_list *stack, t_node *left, t_node *pivot)
{
	// int	distance;

	// distance = measure_distance(stack);

	if (!stack)
			return ;
	// if (left = close to top)
	// {
	close_to_top(stack, left, pivot);
	// }
	// if (left = close to bottom)
	// {
	// 	close_to_bottom(stack, left, pivot);
	// }
}
