/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_sort.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 17:57:03 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/08 18:29:34 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	is_sort(t_linked_list *stack_a)
{
	t_node	*small;
	t_node	*big;
	t_node	*last;

	small = stack_a->top;
	big = small->next;
	last = find_last_node(stack_a);
	while (small != last)
	{
		if (small->index < big->index)
		{
			small = big;
			big = big->next;
		}
		else
			return (0);
	}
	return (1);
}
