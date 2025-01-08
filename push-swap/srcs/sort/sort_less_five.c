/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_less_five.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 19:06:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/08 22:12:03 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

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
	int	min_node;

	min_node = stack_a->top->index;
	while (min_node != 4)
	{
		rotate_a(stack_a);
		min_node = stack_a->top->index;
	}
	if (is_sort(stack_a) == 0)
	{
		push_b(stack_a, stack_b);
		sort_three(stack_a);
		push_a(stack_a, stack_b);
		rotate_a(stack_a);
	}
}

void	sort_five(t_linked_list *stack_a, t_linked_list *stack_b)
{
	int	min_node;

	min_node = stack_a->top->index;
	while (min_node != 5)
	{
		rotate_a(stack_a);
		min_node = stack_a->top->index;
	}
	if (is_sort(stack_a) == 0)
	{
		push_b(stack_a, stack_b);
		sort_four(stack_a, stack_b);
		push_a(stack_a, stack_b);
		rotate_a(stack_a);
	}
}
