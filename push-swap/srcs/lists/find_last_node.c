/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_last_node.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 18:31:14 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/26 17:38:51 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_node	*find_last_node(t_linked_list *stack)
{
	t_node	*current;

	if (!stack)
		return (NULL);
	if (!stack->top)
		return (NULL);
	if (stack->top->next == NULL)
		return (stack->top);
	current = stack->top;
	while (!(current->next == stack->top))
	{
		current = current->next;
	}
	return (current);
}
