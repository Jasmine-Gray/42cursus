/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_list_add_back.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 16:54:42 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/08 20:00:10 by mishimod         ###   ########.fr       */
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

t_linked_list	*double_list_add_back(t_linked_list *stack, t_node *new_node)
{
	t_node	*last;

	if (!stack || !new_node)
	{
		return (NULL);
	}
	if (!(stack->top)) // stack no naka no top ga nai toki
	{
		stack->top = new_node; // stack no top no naka ni new_node wo dainyuu
		return (stack);
	}
	if (stack->top->next == NULL)
	{
		stack->top->next = new_node;
		stack->top->prev = new_node;
		new_node->next = stack->top;
		new_node->prev = stack->top;
	}
	last = find_last_node(stack);
	last->next = new_node;
	stack->top->prev = new_node;
	new_node->next = stack->top;
	new_node->prev = last;
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
    double_list_add_back(&stack, node_1);
    double_list_add_back(&stack, node_2);
    double_list_add_back(&stack, node_3);
    double_list_add_back(&stack, node_4);

	printf("stack=%d->%d->%d->%d\n", stack.top->value, stack.top->next->value, stack.top->next->next->value, stack.top->next->next->next->value); // . ha kouzoutai no senngen ga hennsuu ->ha pointer 
	free(node_1);
	free(node_2);
	free(node_3);
	free(node_4);
    return (0);
}
