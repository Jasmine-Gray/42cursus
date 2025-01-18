/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 20:56:16 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/18 12:06:00 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	free_node(t_linked_list *stack)
{
	t_node	*current;
	t_node	*next;

	if (stack == NULL)
		return ;
	current = stack->top;
	while (current)
	{
		next = current->next;
		detach_node(stack, current);
		free(current);
		current = next;
	}
}

void    free_ft_split(char **words)
{
    int    n;

    n = 0;
    while (words[n] != NULL)
    {
        free(words[n]);
        n++;
    }
    free(words);
}
