/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reverse_rotate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 20:34:33 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/14 20:48:07 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	*reverse_rotate(t_linked_list *stack)
{
	t_node	*node_to_rotate;

	if (!stack)
		return (NULL);
	if (!(stack->top))
		return (NULL);
	node_to_rotate = find_last_node(stack);
	detach_node(stack, node_to_rotate);
	if (!node_to_rotate)
		return (NULL);
	double_list_add_front(stack, node_to_rotate);
	return (stack);
}

void	*reverse_rotate_a(t_linked_list *stack_a)
{
	reverse_rotate(stack_a);
	write(1, "rra\n", 4);
	return (stack_a);
}

void	*reverse_rotate_b(t_linked_list *stack_b)
{
	reverse_rotate(stack_b);
	write(1, "rrb\n", 4);
	return (stack_b);
}

void	*reverse_rotate_both(t_linked_list *stack_a, t_linked_list *stack_b)
{
	reverse_rotate(stack_a);
	reverse_rotate(stack_b);
	write(1, "rrr\n", 4);
	return (stack_a);
}
