/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:14:21 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/14 18:27:07 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	*push(t_linked_list *src, t_linked_list *dest)
{
	t_node	*pushed_node;

	if (!src || !dest)
		return (NULL);
	pushed_node = src->top;
	if (!pushed_node)
		return (NULL);
	detach_node(src, pushed_node);
	double_list_add_front(dest, pushed_node);
	return (dest);
}

void	*push_a(t_linked_list *stack_a, t_linked_list *stack_b)
{
	push(stack_b, stack_a);
	write(1, "pa\n", 3);
	return (stack_b);
}

void	*push_b(t_linked_list *stack_a, t_linked_list *stack_b)
{
	push(stack_a, stack_b);
	write(1, "pb\n", 3);
	return (stack_b);
}
