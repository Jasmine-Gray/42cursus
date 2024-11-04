/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap_left_right.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:56:10 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/04 19:17:04 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	close_to_top(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node *left, t_node *right)
{
	int	count;
	// if (stack_b != NULL)
	// 		return ;

	while (left != stack_a->top)
	{
		push_b(stack_a, stack_b);
	}
	push_b(stack_a, stack_b);
	count = 0;
	printf("swap_LR_right=%d\n", right->value);
	while (!(right == stack_a->top))
	{
		reverse_rotate_a(stack_a);
		count++;
	}
	push_b(stack_a, stack_b);
	swap_b(stack_b);
	retain_value(&left, &right);
	push_a(stack_a, stack_b);
	while (count != 0)
	{
		rotate_a(stack_a);
		count--;
	}
	// printf("1sortleft=%d\n", left->value);
	// printf("1sortright=%d\n", right->value);
	if (stack_b->top == NULL)
			printf("OK\n");
	while (stack_b->top != NULL)
		push_a(stack_a, stack_b);
    print_stack(stack_b);
	// printf("2sortleft=%d\n", left->value);
	// printf("2sortright=%d\n", right->value);
}

static void	close_to_bottom(t_linked_list *stack_a, t_linked_list *stack_b,
		t_node *left, t_node *right)
{
	int	count;

	while (right == stack_b->top)
	{
		reverse_rotate_a(stack_a);
		push_b(stack_a, stack_b);
	}
	count = 0;
	while (!(left = stack_a->top))
	{
		reverse_rotate_a(stack_a);
		count++;
	}
	push_b(stack_a, stack_b);
	swap_b(stack_b);
	retain_value(&left, &right);
	push_a(stack_a, stack_b);
	while (count != 0)
	{
		rotate_a(stack_a);
		count--;
	}
	while (stack_b)
		push_a(stack_a, stack_b);
}

void	swap_left_right(t_linked_list *stack_a, t_linked_list *stack_b, t_node *left, t_node *right)
{
	int				distance;

	// printf("%d", stack_b->top->value);
	distance = measure_distance(stack_a, left);
	if (distance <= 75)
	{
		close_to_top(stack_a, stack_b, left, right);
	}
	if (distance > 75)
	{
		close_to_bottom(stack_a, stack_b, left, right);
	}
}
