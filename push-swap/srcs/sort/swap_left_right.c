/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap_left_right.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:56:10 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/22 17:15:45 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void swap_left_right(t_linked_list *stack, t_node *left, t_node *right)
{
		t_linked_list *stack_a;		
		t_linked_list *stack_b;		
		int count;

		if (close from top)
		{

				while (!(left->next == stack_a->top))
				{
						push_b(stack_a, stack_b);
				}
				count = 0;
				while (!(right = stack_a->top))
				{
						reverse_rotate_a(stack_a);
						count++;
				}
				push_b(stack_a, stack_b);
				swap_b(stack_b);
				push_a(stack_a, stack_b);
				while(count)
				{
						rotate_a(stack_a);
				}
		}

		if (close from bottom)
		{

				while (!(left->next == stack_a->top))
				{
						push_b(stack_a, stack_b);
				}
				count = 0;
				while (!(right = stack_a->top))
				{
						reverse_rotate_a(stack_a);
						count++;
				}
				push_b(stack_a, stack_b);
				swap_b(stack_b);
				push_a(stack_a, stack_b);
				while(count)
				{
						rotate_a(stack_a);
				}
		}
}

