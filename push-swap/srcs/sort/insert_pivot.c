/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   insert_pivot.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 20:58:19 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/04 21:41:21 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	close_to_top(t_linked_list *stack_a, t_linked_list *stack_b, t_node **left)
{
	if (!stack_a)
		return ;
	while (*left != stack_a->top)
	{
		push_b(stack_a, stack_b);
	}
	reverse_rotate_a(stack_a);
	push_a(stack_a, stack_b);
}

static void	close_to_bottom(t_linked_list *stack_a, t_linked_list *stack_b, t_node **left)
{
	int				count;

	reverse_rotate_a(stack_a);
	push_b(stack_a, stack_b);
	count = 1;
	while (*left != stack_a->top)
	{
		reverse_rotate_a(stack_a);
		count++;
	}
	push_b(stack_a, stack_b);
	while (count)
	{
		rotate_a(stack_a);
		count--;
	}
	if (stack_b != NULL)
		return ;
}

void	insert_pivot(t_linked_list *stack_a, t_linked_list *stack_b, t_node **left)
{
	int				distance;

	distance = measure_distance(stack_a, left);
	if (!stack_a)
		return ;
	if (distance <= 75)
	{
		close_to_top(stack_a, stack_b, left);
	}
	if (distance > 75)
	{
			close_to_bottom(stack_a, stack_b, left);
	}
}
