/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_node.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 18:30:17 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/14 18:27:20 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	count_node(t_linked_list *stack)
{
	int		num;
	t_node	*current;

	if (!stack)
		return (0);
	current = stack->top;
	if (current == NULL)
		return (0);
	if (current->next == NULL)
		return (1);
	num = 0;
	while (current != stack->top || num == 0)
	{
		num++;
		current = current->next;
	}
	return (num);
}
