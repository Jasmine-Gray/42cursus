/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 13:00:18 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/11 19:53:14 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	main(void)
{
	t_linked_list	stack;
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
