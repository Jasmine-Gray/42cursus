/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_list_add_front.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 19:01:04 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/14 21:07:32 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	stack_top_is_one(t_linked_list *stack, t_node *new_node)
{
	t_node	*tmp;

	tmp = stack->top;
	tmp->prev = new_node;
	tmp->next = new_node;
	stack->top = new_node;
	stack->top->next = tmp;
	stack->top->prev = tmp;
}

t_linked_list	*double_list_add_front(t_linked_list *stack, t_node *new_node)
{
	t_node	*tmp;
	t_node	*last;

	if (!stack || !new_node)
		return (NULL);
	if (!(stack->top))
	{
		stack->top = new_node;
		return (stack);
	}
	if (stack->top->prev == NULL)
	{
		stack_top_is_one(stack, new_node);
		return (stack);
	}
	tmp = stack->top;
	last = find_last_node(stack);
	tmp->prev = new_node;
	stack->top = new_node;
	new_node->next = tmp;
	new_node->prev = last;
	last->next = new_node;
	return (stack);
}
