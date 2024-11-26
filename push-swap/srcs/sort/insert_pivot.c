/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   insert_pivot.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 20:58:19 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/26 17:43:55 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	close_to_top(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left)
{
	int	count;

	if (!stack_a)
		return ;
	count = 0;
	while (*left != stack_a->top)
	{
		push_b(stack_a, stack_b);
		count++;
	}
	reverse_rotate_a(stack_a);
	while (count != 0)
	{
		push_a(stack_a, stack_b);
		count--;
	}
}

static void	close_to_bottom(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left)
{
	int	count;

	reverse_rotate_a(stack_a);
	push_b(stack_a, stack_b);
	count = 1;
	while (*left != stack_a->top)
	{
		reverse_rotate_a(stack_a);
		count++;
	}
	push_a(stack_a, stack_b);
	while (count)
	{
		rotate_a(stack_a);
		count--;
	}
}

void	insert_pivot(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left, t_node **pivot)
{
	int		distance;
	t_node	*last;

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
	last = find_last_node(stack_a);
	*pivot = last;
}
