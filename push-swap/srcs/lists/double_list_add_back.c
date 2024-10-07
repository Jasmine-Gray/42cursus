/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_list_add_back.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 16:54:42 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/07 22:55:18 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_stack *double_list_add_back(t_stack *new_node)
{
		t_stack *last;
		t_stack **list;
		

		if (!list || !new_node)
		{
				return ;
		}

		if (!*list)
		{
				*list = new_node;
		}


		if (stackがtopだけの場合)
		{
		}

		else if (topにprevの値が入っている場合)
		{

		}
		last->next = new_node;
		new_node->prev = last;
		return ;
}
