/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node_new.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:04:29 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/23 01:50:16 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_node	*node_new(int value)
{
	t_node	*new;

	new = malloc(sizeof(t_node));
	if (!new)
	{
		return (NULL);
		// free (new);
	}
	new->value = value;
	new->prev = NULL;
	new->next = NULL;
	new->index = 0;
	return (new);
	// free(new);
}
