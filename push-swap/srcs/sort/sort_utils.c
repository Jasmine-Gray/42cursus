/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 20:53:37 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/26 16:01:48 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	retain_value(t_node *left, t_node *right)
{
	t_node	*tmp;

	tmp = left;
	left = right;
	right = tmp;
}

void	retain_pivot(t_node *left, t_node *pivot)
{
	t_node	*tmp;

	tmp = left;
	left = pivot;
	pivot = tmp;
}

int	measure_distance(t_linked_list *stack, t_node *left)
{
	t_linked_list	*stack_a;
	int				count;
	int				distance;
	int				result;
	t_node *current;

	stack_a = stack;
	if (!stack_a)
		return 0;
	count = count_node(stack_a);
	distance = 0;
	current = stack_a->top;
	while (!left)
	{
		distance++;
		current = current->next;
	}
	result = (distance / count) * 100;
	return (result);
}
