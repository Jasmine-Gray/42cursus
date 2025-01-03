/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 19:40:27 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/04 08:51:36 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	radix_sort(t_linked_list *stack_a, t_linked_list *stack_b)
{
	size_t	stack_size;
	size_t	len;

	stack_size = count_node(stack_a);
	len = 0;
	while ()// work_in_progress)
	{
		if (is_sort(stack_a))
			break ;
		while (len < stack_size)
		{
			if (((>>) & 1) == 0) // work_in_progress
				push_b(stack_a, stack_b);
			else
				rotate_a(stack_a);
			len++;
		}
		while (stack_b)
			push_a(stack_a, stack_b);
	}
}

void	sort(t_linked_list *stack_a, t_linked_list *stack_b)
{
	size_t	stack_size;

	stack_size = count_node(stack_a);
	if (is_sort(stack_a))
		return ;
	if (stack_size == 2)
		swap_a(stack_a);
	else if (stack_size == 3)
		sort_three(stack_a);
	else if (stack_size == 4)
		sort_four(stack_a); //wip
	else if (stack_size == 5)
		sort_five(stack_a); //wip
	else
		radix_sort(stack_a, stack_b);
}
