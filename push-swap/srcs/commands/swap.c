/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 19:19:02 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/15 12:05:55 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	*swap(t_linked_list *stack)
{
	t_node	*first;
	t_node	*second;

	if (!stack)
		return (NULL);
	if (!(stack->top))
		return (NULL);
	if (!(stack->top->next))
		return (NULL);
	first = stack->top;
	second = first->next;
	detach_node(stack, second);
	double_list_add_front(stack, second);
	return (stack);
}

void	*swap_a(t_linked_list *stack_a)
{
	swap(stack_a);
	write(1, "sa\n", 3);
	return (stack_a);
}

void	*swap_b(t_linked_list *stack_b)
{
	swap(stack_b);
	write(1, "sb\n", 3);
	return (stack_b);
}

void	*swap_both(t_linked_list *stack_a, t_linked_list *stack_b)
{
	swap(stack_a);
	swap(stack_b);
	write(1, "ss\n", 3);
	return (stack_a);
}
