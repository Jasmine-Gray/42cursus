/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 17:15:18 by mishimod          #+#    #+#             */
/*   Updated: 2024/10/16 01:14:43 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	build_stack(int argc, const char **argv_str, t_linked_list *stack)
{
	t_node	*tmp;
	size_t			i;


	i = 0;
	while (argv_str[i])
	{
		tmp = node_new(ft_atoi(argv_str[i]));
		if (!(argc == 2))
			return ;
		if (!tmp)
			return ;
		double_list_add_back(stack, tmp);
 		i++;
	}
	return ;
}
