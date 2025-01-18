/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_duplicate.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 15:59:20 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/18 22:41:27 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

bool	check_duplicate(t_linked_list *stack_a, int size)
{
	t_node	*current;
	t_node	*target;
	int		size_b;

	current = stack_a->top;
	target = current->next;
	while (size != 0)
	{
		size_b = size - 1;
		target = current->next;
		while (size_b != 0)
		{
			if (current->value == target->value)
			{
				ft_putstr_fd("Error\n", 2);
				return (true);
			}
			target = target->next;
			size_b--;
		}
		current = current->next;
		size--;
	}
	return (false);
}
