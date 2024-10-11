/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_list_add_front.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 00:36:43 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/11 16:51:20 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_linked_list	*double_list_add_front(t_linked_list *stack, t_node *new_node)
{
	if (!stack || !new_node) 
		return (NULL);
	if (!(stack->top))
		stack->top = new_node;
		return (stack);	
	if (stack->top->prev == NULL)
	{
		stack->top->prev = new_node;
		stack->top->next = new_node;
		new_node->prev = stack->top;
		new_node->next = stack->top;
	}

	return (stack);
}
