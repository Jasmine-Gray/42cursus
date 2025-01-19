/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 13:09:13 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/18 11:23:55 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	print_stack(t_linked_list *stack)
{
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

// void	print_stack_index(t_linked_list *stack)
// {
// 	t_node	*current;
// 	int		num;
// 
// 	printf("list:");
// 	if (!(stack->top))
// 	{
// 		printf("(null)\n");
// 		return ;
// 	}
// 	current = stack->top;
// 	num = 0;
// 	while (current != stack->top || num == 0)
// 	{
// 		printf("%d", current->index);
// 		num++;
// 		current = current->next;
// 		if (current == stack->top || !current)
// 			break ;
// 		printf("->");
// 	}
// 	printf("\n");
// }
