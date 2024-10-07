/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 13:16:21 by mishimod          #+#    #+#             */
/*   Updated: 2024/09/22 19:05:57 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

typedef struct s_node
{
		int value;
		struct s_node *prev;
		struct s_node *next;

} t_node

typedef struct s_linked_list
{
		t_node *top;

} t_linked_list

#endif
