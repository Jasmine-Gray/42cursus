/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 17:15:18 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/16 15:29:41 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	build_stack(int argc, const char **argv_str, t_linked_list *stack)
{
	t_node	*tmp;
	size_t	i;

	if ((argc < 2) || (!argv_str) || (!stack))
		return ;
	i = 0;
	while (argc == 2 && argv_str[i])
	{
		tmp = node_new(ft_atoi(argv_str[i]));
		if (!tmp || (tmp->value < INT_MIN || tmp->value > INT_MAX))
			return ;
		double_list_add_back(stack, tmp);
		i++;
	}
	i = 1;
	while (argc > 2 && argv_str[i])
	{
		tmp = node_new(ft_atoi(argv_str[i]));
		if (!tmp || (tmp->value < INT_MIN || tmp->value > INT_MAX))
			return ;
		double_list_add_back(stack, tmp);
		i++;
	}
	return ;
}
