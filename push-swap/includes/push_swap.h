/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 13:16:21 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/08 18:40:18 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

#include <stdio.h>
#include <stdlib.h>

typedef struct s_node
{
		int value;
		struct s_node *prev;
		struct s_node *next;

} t_node;

typedef struct s_linked_list
{
		t_node *top;

} t_linked_list;

t_node *node_new(int value);
t_linked_list *double_list_add_back(t_linked_list *stack, t_node *new_node);

#endif
