/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   insert_pivot.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 20:58:19 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/28 21:25:12 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	close_to_top(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left, t_node **pivot)
{
	int		num;
	int		count;
	t_node	*last;

	last = find_last_node(stack_a);
	if (!stack_a)
		return ;
	while (*left != stack_a->top)
	{
		push_b(stack_a, stack_b);
	}
	num = 0;
	while (*pivot != last) // infinite loop
	{
		reverse_rotate_a(stack_a);
		print_stack(stack_a);
		printf("pivot=%d\n", (*pivot)->value);
		printf("last=%d\n", last->value);
		num++;
	}
	count = 0;
	while (num)
	{
		push_b(stack_a, stack_b);
		num--;
		count++;
	}
	reverse_rotate_a(stack_a);
	while (count != 0)
	{
		push_a(stack_a, stack_b);
		rotate_a(stack_a);
		count--;
	}
	//  while (stack_b != NULL)
	//  {
	//  	push_a(stack_a, stack_b);
	//  }
}

static void	close_to_bottom(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left, t_node **pivot)
{
	int		count;
	int		num;
	t_node	*last;

	last = find_last_node(stack_a);
	count = 0;
	while ((*pivot) != last)
	{
		reverse_rotate_a(stack_a);
		count++;
	}
	while (count != 0)
	{
		push_b(stack_a, stack_b);
		count--;
	}
	reverse_rotate_a(stack_a);
	push_b(stack_a, stack_b);
	num = 1;
	while (*left != stack_a->top)
	{
		reverse_rotate_a(stack_a);
		num++;
	}
	push_a(stack_a, stack_b);
	while (num)
	{
		rotate_a(stack_a);
		num--;
	}
	while (stack_b != NULL)
	{
		push_a(stack_a, stack_b);
		rotate_a(stack_a);
	}
}

void	insert_pivot(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node **left, t_node **pivot)
{
	int	distance;

	distance = measure_distance(stack_a, left);
	if (!stack_a)
		return ;
	if (distance <= 75)
	{
		close_to_top(stack_a, stack_b, left, pivot);
	}
	if (distance > 75)
	{
		close_to_bottom(stack_a, stack_b, left, pivot);
	}
}
