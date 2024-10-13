/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   detach_node.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 17:19:53 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/13 20:58:17 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	detach_node(t_linked_list *stack, t_node *node_to_detach)
{
	t_node	*prev_node;
	t_node	*next_node;
	int		count;

	count = count_node(stack);
	if (count == 1)
	{
		return ;
	}
	if (node_to_detach == stack->top)
	{
		stack->top = stack->top->next;
	}
	next_node = node_to_detach->next;
	prev_node = node_to_detach->prev;
	node_to_detach->next = NULL;
	node_to_detach->prev = NULL;
	if (count == 2)
	{
		next_node->next = NULL;
		next_node->prev = NULL;
		return ;
	}
	next_node->prev = prev_node;
	prev_node->next = next_node;
}
