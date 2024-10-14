/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_list_add_back.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 18:40:12 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/14 20:51:36 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_linked_list	*double_list_add_back(t_linked_list *stack, t_node *new_node)
{
	t_node	*last;

	if (!stack || !new_node)
	{
		return (NULL);
	}
	if (!(stack->top))
	{
		stack->top = new_node;
		return (stack);
	}
	if (stack->top->next == NULL)
	{
		stack->top->next = new_node;
		stack->top->prev = new_node;
		new_node->next = stack->top;
		new_node->prev = stack->top;
		return (stack);
	}
	last = find_last_node(stack);
	last->next = new_node;
	stack->top->prev = new_node;
	new_node->next = stack->top;
	new_node->prev = last;
	return (stack);
}
