/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 17:15:18 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/18 11:29:46 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	build_stack(int argc, const char **argv_str, t_linked_list *stack)
{
	t_node	*tmp;
	size_t	i;
	char	*endptr;

	if ((argc < 2) || (!argv_str) || (!stack))
		return(0) ;
	if (argc == 2)
		i = 0;
	if (argc > 2)
		i = 1;
	while (argv_str[i])
	{
		ft_strtol(argv_str[i], &endptr);
		if (*endptr != '\0')
		{
			ft_putstr_fd("Error\n", 2);
			return (0);
		}
		endptr = NULL;
		tmp = node_new((int)ft_strtol(argv_str[i], &endptr));
		if (!tmp || (tmp->value < INT_MIN || tmp->value > INT_MAX))
		{
			free(tmp);
			return (0);
		}
		double_list_add_back(stack, tmp);
		i++;
	}
	return (1);
}
