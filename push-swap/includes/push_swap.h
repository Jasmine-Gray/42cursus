/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 13:16:21 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/04 19:16:44 by mishimod         ###   ########.fr       */
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

// lists
t_node				*node_new(int value);
t_node				*find_last_node(t_linked_list *stack);
t_linked_list		*double_list_add_back(t_linked_list *stack,
						t_node *new_node);
t_linked_list		*double_list_add_front(t_linked_list *stack,
						t_node *new_node);
int					count_node(t_linked_list *stack);
void				detach_node(t_linked_list *stack, t_node *node_to_detach);
// commands
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
void				*swap_a(t_linked_list *stack_a);
void				*swap_b(t_linked_list *stack_b);
void				*swap_both(t_linked_list *stack_a, t_linked_list *stack_b);
// utils
const char			**check_args(int argc, const char **argv);
void				build_stack(int argc, const char **argv_str,
						t_linked_list *stack);
void				print_stack(t_linked_list *stack);
// sort

void				sort(t_linked_list *stack_a, t_linked_list *stack_b);
void				swap_left_pivot(t_linked_list *stack_a,
						t_linked_list *stack_b, t_node *left, t_node *pivot);
void				swap_left_right(t_linked_list *stack_a,
						t_linked_list *stack_b, t_node *left, t_node *right);
void				retain_value(t_node **left, t_node **right);
void				retain_pivot(t_node *left, t_node *pivot);
int					measure_distance(t_linked_list *stack, t_node *left);
void				insert_pivot(t_linked_list *stack, t_node *left);

#endif
