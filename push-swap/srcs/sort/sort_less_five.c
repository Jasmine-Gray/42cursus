/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_less_five.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 19:06:14 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/04 07:49:42 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	sort_two(t_stack *stack_a)
{
	if (stack_a->top->index > stack_a->top->next->index)
		sa(stack);
}

void	sort_three(t_stack *stack_a)
{
	int	a;
	int	b;
	int	c;

	a = stack_a->top->index;
	b = stack_a->top->next->index;
	c = stack_a->top->next->next->index;
	if (a > b && b < c && a < c)
		sa(stack_a);
	else if (a > b && b > c)
	{
		sa(stack_a);
		ra(stack_a);
	}
	else if (a > b && b < c && a > c)
		ra(stack_a);
	else if (a < b && b > c && a < c)
	{
		sa(stack_a);
		ra(stack_a);
	}
	else if (a < b && b > c && a > c)
		rra(stack_a);
}

void	sort_four(t_stack *stack_a)
{
	int	min_pos;

	min_pos = find_min_position(stack_a);
	while (min_pos > 0)
	{
		ra(stack_a);
		min_pos--;
	}
	pb(stack_a, stack_b);
	sort_three(stack);
	pa(stack);
}
