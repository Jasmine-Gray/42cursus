/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotate.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 19:16:20 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/14 20:48:17 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	*rotate(t_linked_list *stack)
{
	t_node	*node_to_rotate;

	if (!stack)
		return (NULL);
	if (!(stack->top))
		return (NULL);
	node_to_rotate = stack->top;
	detach_node(stack, node_to_rotate);
	if (!node_to_rotate)
		return (NULL);
	double_list_add_back(stack, node_to_rotate);
	return (stack);
}

void	*rotate_a(t_linked_list *stack_a)
{
	rotate(stack_a);
	write(1, "ra\n", 3);
	return (stack_a);
}

void	*rotate_b(t_linked_list *stack_b)
{
	rotate(stack_b);
	write(1, "rb\n", 3);
	return (stack_b);
}

void	*rotate_both(t_linked_list *stack_a, t_linked_list *stack_b)
{
	rotate(stack_a);
	rotate(stack_b);
	write(1, "rr\n", 3);
	return (stack_a);
}
