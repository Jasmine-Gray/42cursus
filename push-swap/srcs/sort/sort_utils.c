/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 20:53:37 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/24 21:29:29 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"


void	retain_value(t_node *left, t_node *right)
{
	t_node *tmp;

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

// int	measure_distance(t_linked_list *stack, t_node *left)
// {
// 	t_linked_list	*stack_a;
// 	int				distance;
// 
//     count_node(stack_a);
// 
// 	stack_a = stack;
// 	distance = 0;
// 	if (!stack_a)
// 		return ;
// 	while (!last)
// 	{
// 		distance++;
// 		stack_a->top->next;
// 	}
// 	return (distance);
// }
