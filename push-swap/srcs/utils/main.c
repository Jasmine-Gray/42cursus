/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 16:54:55 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/18 22:34:06 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	main(int argc, char const *argv[])
{
	t_linked_list	stack_a;
	const char		**argv_str;
	t_linked_list	stack_b;
	int				flag;

	if (argc < 2)
		return (0);
	stack_a.top = NULL;
	stack_b.top = NULL;
	argv_str = check_args(argc, argv);
	flag = build_stack(argc, argv_str, &stack_a);
	if (flag == 0 || check_duplicate(&stack_a, count_node(&stack_a)))
		return (free_node(&stack_a), 1);
	index_stack(&stack_a);
	if (is_sort(&stack_a) == 1 || count_node(&stack_a) == 1)
		return (free_node(&stack_a), 0);
	sort(&stack_a, &stack_b);
	return (free_node(&stack_a), 0);
}
