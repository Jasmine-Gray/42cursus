/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 16:54:55 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/04 07:11:58 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	print_stack(t_linked_list *stack)
{
	t_node	*current;
	int		num;

	printf("list:");
	if (!(stack->top))
	{
		printf("(null)\n");
		return ;
	}
	current = stack->top;
	num = 0;
	while (current != stack->top || num == 0)
	{
		printf("%d", current->value);
		num++;
		current = current->next;
		if (current == stack->top || !current)
			break ;
		printf("->");
	}
	printf("\n");
}

static void	print_stack_index(t_linked_list *stack)
{
	t_node	*current;
	int		num;

	printf("list:");
	if (!(stack->top))
	{
		printf("(null)\n");
		return ;
	}
	current = stack->top;
	num = 0;
	while (current != stack->top || num == 0)
	{
		printf("%d", current->index);
		num++;
		current = current->next;
		if (current == stack->top || !current)
			break ;
		printf("->");
	}
	printf("\n");
}

int	main(int argc, char const *argv[])
{
	t_linked_list	stack_a;
	const char		**argv_str;
	t_linked_list	stack_b;

	stack_a.top = NULL;
	stack_b.top = NULL;
	argv_str = check_args(argc, argv);
	build_stack(argc, argv_str, &stack_a);
	// print_stack(&stack_a);
	index_stack(&stack_a);
	// print_stack_index(&stack_a);
	// sort(&stack_a, &stack_b);
    radix_sort(&stack_a, &stack_b);
	return (0);
}
