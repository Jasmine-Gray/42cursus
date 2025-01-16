/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 13:16:21 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/16 13:32:42 by mishimod         ###   ########.fr       */
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
	int				index;
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
// void				print_stack(t_linked_list *stack);

// sort
void				sort(t_linked_list *stack_a, t_linked_list *stack_b);
int					is_sort(t_linked_list *stack_a);
void				index_stack(t_linked_list *stack_a);
void				sort_three(t_linked_list *stack_a);
void				sort_four(t_linked_list *stack_a, t_linked_list *stack_b);
void				sort_five(t_linked_list *stack_a, t_linked_list *stack_b);

// error
void				ft_error(t_linked_list *stack);
void				free_stack(t_linked_list *stack);

#endif
