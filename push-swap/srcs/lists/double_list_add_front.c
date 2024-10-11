`* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_list_add_front.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 00:36:43 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/11 18:32:48 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_linked_list	*double_list_add_front(t_linked_list *stack, t_node *new_node)
{
	t_node	*tmp;
	t_node	*last;

	if (!stack || !new_node)
		return (NULL);
	if (!(stack->top))
	{
		stack->top = new_node;
		return (stack);
	}
	if (stack->top->prev == NULL)
	{
		tmp = stack->top;
		tmp->prev = new_node;
		tmp->next = new_node;
		stack->top = new_node;
		stack->top->next = tmp;
		stack->top->prev = tmp;
		return (stack);
	}
	tmp = stack->top;
	last = find_last_node(stack);
	tmp->prev = new_node;
	stack->top = new_node;
	new_node->next = tmp;
	new_node->prev = last;
	last->next = new_node;
	return (stack);
}

int	main(void)
{
	t_linked_list	stack;
	t_linked_list	*result;
	t_node			*node_1;
	t_node			*node_2;
	t_node			*node_3;
	t_node			*node_4;

	stack.top = NULL;
	node_1 = node_new(12);
	node_2 = node_new(24);
	node_3 = node_new(42);
	node_4 = node_new(26);
	double_list_add_front(&stack, node_1);
	printf("stack=%d\n", stack.top->value);
	double_list_add_front(&stack, node_2);
	printf("stack=%d\n", stack.top->value);
	double_list_add_front(&stack, node_3);
	printf("stack=%d\n", stack.top->value);
	double_list_add_front(&stack, node_4);
	printf("stack=%d\n", stack.top->value);
	free(node_1);
	free(node_2);
	free(node_3);
	free(node_4);
	return (0);
}
