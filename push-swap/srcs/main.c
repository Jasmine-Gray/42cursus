/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 13:00:18 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/15 22:57:32 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//     #include "push_swap.h"
//     
//     static void print_stack(t_linked_list *stack)
//     {
//     	t_node *current;
//     	int num;
//     
//     	printf("list:");
//     	if (!(stack->top))
//     {
//     	printf("(null)\n");
//     		return ;
//     }
//     	current = stack->top;
//     	num = 0;
//     	while (current != stack->top || num == 0)
//     	{
//     		printf("%d", current->value);
//     		num++;
//     		current = current->next;
//     		if(current == stack->top || !current)
//     			break;
//     		printf("->");
//     	}
//     	printf("\n");
//     }
//     int	main(void)
//     {
//     	t_linked_list	stack_a;
//     	t_linked_list	stack_b;
//     	t_node			*node_1;
//     	t_node			*node_2;
//     	t_node			*node_3; 
//     	t_node			*node_4;
//     	int 			count;
//     
//     	stack_a.top = NULL;
//     	stack_b.top = NULL;
//     	node_1 = node_new(12);
//     	node_2 = node_new(24);
//     	node_3 = node_new(42);
//     	node_4 = node_new(26);
//     
//     	//printf("===================================\n");
//     	//printf("double_list_add_front\n");
//     
//     	//double_list_add_front(&stack_a, node_1);
//     	//double_list_add_front(&stack_a, node_2);
//     	//double_list_add_front(&stack_a, node_3);
//     	//double_list_add_front(&stack_a, node_4);
//     	//print_stack(&stack_a);
//     
//     	printf("===================================\n");
//     	printf("double_list_add_back -stack_a-\n");
//     
//     	double_list_add_back(&stack_a, node_1);
//     	double_list_add_back(&stack_a, node_2);
//     	double_list_add_back(&stack_a, node_3);
//     	double_list_add_back(&stack_a, node_4);
//     	print_stack(&stack_a);
//     
//     	// printf("===================================\n");
//     	// printf("double_list_add_back -stack_b-\n");
//     
//     	// double_list_add_back(&stack_b, node_1);
//     	// double_list_add_back(&stack_b, node_2);
//     	// double_list_add_back(&stack_b, node_3);
//     	// double_list_add_back(&stack_b, node_4);
//     	// print_stack(&stack_b);
//     
//     	printf("\n===================================\n");
//     	printf("push_b\n");
//     
//     	push_b(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	push_b(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	push_b(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	push_b(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	printf("===================================\n");
//     	printf("push_a\n");
//     
//     	push_a(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	push_a(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	// push_a(&stack_a, &stack_b);
//     	// print_stack(&stack_a);
//     	// print_stack(&stack_b);
//     
//     	// push_a(&stack_a, &stack_b);
//     	// print_stack(&stack_a);
//     	// print_stack(&stack_b);
//     
//     	printf("===================================\n");
//     	printf("rotate_a\n");
//     
//     	rotate_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	rotate_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	rotate_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	rotate_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	printf("===================================\n");
//     	printf("rotate_b\n");
//     
//     	rotate_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	rotate_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	rotate_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	rotate_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	printf("===================================\n");
//     	printf("rotate_both\n");
//     
//     	rotate_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	rotate_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	rotate_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	rotate_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     
//     	printf("===================================\n");
//     	printf("reverse_rotate_a\n");
//     
//     	reverse_rotate_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	reverse_rotate_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	reverse_rotate_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	reverse_rotate_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	printf("===================================\n");
//     	printf("reverse_rotate_b\n");
//     
//     	reverse_rotate_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	reverse_rotate_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	reverse_rotate_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	reverse_rotate_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	printf("===================================\n");
//     	printf("reverse_rotate_both\n");
//     
//     	reverse_rotate_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	reverse_rotate_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	reverse_rotate_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	reverse_rotate_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     
//     	printf("===================================\n");
//     	printf("swap_a\n");
//     
//     	swap_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	swap_a(&stack_a);
//     	print_stack(&stack_a);
//     
//     	// swap_a(&stack_a);
//     	// print_stack(&stack_a);
//     
//     	// swap_a(&stack_a);
//     	// print_stack(&stack_a);
//     
//     	printf("===================================\n");
//     	printf("swap_b\n");
//     
//     	swap_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	swap_b(&stack_b);
//     	print_stack(&stack_b);
//     
//     	// swap_b(&stack_b);
//     	// print_stack(&stack_b);
//     
//     	// swap_b(&stack_b);
//     	// print_stack(&stack_b);
//     
//     	printf("===================================\n");
//     	printf("swap_both\n");
//     
//     	swap_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	swap_both(&stack_a, &stack_b);
//     	print_stack(&stack_a);
//     	print_stack(&stack_b);
//     
//     	// swap_both(&stack_a, &stack_b);
//     	// print_stack(&stack_a);
//     	// print_stack(&stack_b);
//     
//     	// swap_both(&stack_a, &stack_b);
//     	// print_stack(&stack_a);
//     	// print_stack(&stack_b);
//     
//     
//     	printf("===================================\n");
//     	printf("count_node\n");
//     
//     	count = 0;
//     	count = count_node(&stack_a);
//     	printf("count=%d\n", count);
//     
//     
//     	// printf("===================================\n");
//     	// printf("detach_node\n");
//     
//         // detach_node(&stack_a, stack_a.top);
//     	// printf("stack_a->top=%d\n", stack_a.top->value);
//     	// printf("stack_a->top->next=%p\n", stack_a.top->next);
//     	// printf("stack_a->top->next->next=%d\n", stack_a.top->next->next->value);
//     	// printf("stack_a->top->next->next->next=%d\n", stack_a.top->next->next->next->value);
//     
//     	free(node_1);
//     	free(node_2);
//     	free(node_3);
//     	free(node_4);
//     	return (0);
//     }
