/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 13:16:21 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/14 20:48:11 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "../libft/libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_node
{
	int				value;
	struct s_node	*prev;
	struct s_node	*next;

}					t_node;

typedef struct s_linked_list
{
	t_node			*top;

}					t_linked_list;

t_node				*node_new(int value);
t_node				*find_last_node(t_linked_list *stack);
t_linked_list		*double_list_add_back(t_linked_list *stack,
						t_node *new_node);
t_linked_list		*double_list_add_front(t_linked_list *stack,
						t_node *new_node);
int					count_node(t_linked_list *stack);
void				detach_node(t_linked_list *stack, t_node *node_to_detach);
void				*push_a(t_linked_list *stack_a, t_linked_list *stack_b);
void				*push_b(t_linked_list *stack_a, t_linked_list *stack_b);
void				*rotate_a(t_linked_list *stack_a);
void				*rotate_b(t_linked_list *stack_b);
void				*rotate_both(t_linked_list *stack_a,
						t_linked_list *stack_b);
void				*reverse_rotate_a(t_linked_list *stack_a);
void				*reverse_rotate_b(t_linked_list *stack_b);
void				*reverse_rotate_both(t_linked_list *stack_a,
						t_linked_list *stack_b);

#endif
