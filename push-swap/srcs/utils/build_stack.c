/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 17:15:18 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/19 14:53:07 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	validate_and_convert(const char *str, t_node **node)
{
	char	*endptr;
	long	val;

	val = ft_strtol(str, &endptr);
	if (*endptr != '\0')
		return (0);
	*node = node_new((int)val);
	if (!(*node) || ((*node)->value < INT_MIN || (*node)->value > INT_MAX))
	{
		free(*node);
		return (0);
	}
	return (1);
}

int	build_stack(int argc, const char **argv_str, t_linked_list *stack)
{
	t_node	*tmp;
	size_t	i;

	if ((argc < 2) || (!argv_str) || (!stack))
		return (0);
	if (argc == 2)
		i = 0;
	if (argc > 2)
		i = 1;
	while (argv_str[i])
	{
		if (!validate_and_convert(argv_str[i++], &tmp))
		{
			ft_putstr_fd("Error\n", 2);
			if (argc == 2)
				free_ft_split((char **)argv_str);
			return (0);
		}
		double_list_add_back(stack, tmp);
	}
	if (argc == 2)
		free_ft_split((char **)argv_str);
	return (1);
}
