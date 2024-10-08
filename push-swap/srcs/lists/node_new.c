/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node_new.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 13:04:29 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/08 18:36:40 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_node *node_new(int value)
{
		t_node *new;

		new = malloc(sizeof(t_node));
		if (!new)
				return (NULL);
		new->value = value;
		new->prev = NULL;
		new->next = NULL;
		return (new);
}

// int main(void)
// {
// 		t_node *result;
// 		result = node_new(42);
// 		printf("result=%d\n", result->value);
// 		return (0);
// }
