/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_less_five.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 19:06:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/18 22:30:22 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	push_a_and_rotate_a(t_linked_list *stack_a, t_linked_list *stack_b)
{
	push_a(stack_a, stack_b);
	rotate_a(stack_a);
}

void	sort_three(t_linked_list *stack_a)
{
	int	a;
	int	b;
	int	c;

	a = stack_a->top->index;
	b = stack_a->top->next->index;
	c = stack_a->top->next->next->index;
	if (a > b && b < c && a < c)
		swap_a(stack_a);
	else if (a > b && b > c)
	{
		swap_a(stack_a);
		reverse_rotate_a(stack_a);
	}
	else if (a > b && b < c && a > c)
		rotate_a(stack_a);
	else if (a < b && b > c && a < c)
	{
		swap_a(stack_a);
		rotate_a(stack_a);
	}
	else if (a < b && b > c && a > c)
		reverse_rotate_a(stack_a);
}

void	sort_four(t_linked_list *stack_a, t_linked_list *stack_b)
{
	t_node	*min_node;
	int		count;

	min_node = stack_a->top;
	count = 0;
	while (min_node->index != 4)
	{
		min_node = min_node->next;
		count++;
	}
	if (count < 2)
	{
		while (min_node->index != stack_a->top->index)
			rotate_a(stack_a);
		push_b(stack_a, stack_b);
	}
	if (count >= 2)
	{
		while (min_node->index != stack_a->top->index)
			reverse_rotate_a(stack_a);
		push_b(stack_a, stack_b);
	}
	if (is_sort(stack_a) == 0)
		sort_three(stack_a);
	push_a_and_rotate_a(stack_a, stack_b);
}

void	sort_five(t_linked_list *stack_a, t_linked_list *stack_b)
{
	t_node	*min_node;
	int		count;

	min_node = stack_a->top;
	count = 0;
	while (min_node->index != 5)
	{
		min_node = min_node->next;
		count++;
	}
	if (count < 3)
	{
		while (min_node->index != stack_a->top->index)
			rotate_a(stack_a);
		push_b(stack_a, stack_b);
	}
	if (count >= 3)
	{
		while (min_node->index != stack_a->top->index)
			reverse_rotate_a(stack_a);
		push_b(stack_a, stack_b);
	}
	if (is_sort(stack_a) == 0)
		sort_four(stack_a, stack_b);
	push_a_and_rotate_a(stack_a, stack_b);
}
