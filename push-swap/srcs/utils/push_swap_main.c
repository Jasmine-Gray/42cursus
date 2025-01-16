/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 16:54:55 by mishimod          #+#    #+#             */
/*   Updated: 2025/01/16 15:31:16 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	main(int argc, char const *argv[])
{
	t_linked_list	stack_a;
	const char		**argv_str;
	t_linked_list	stack_b;

	if (argc < 2)
		return (0);
	stack_a.top = NULL;
	stack_b.top = NULL;
	argv_str = check_args(argc, argv);
	build_stack(argc, argv_str, &stack_a);
	// print_stack(&stack_a);
	index_stack(&stack_a);
	// print_stack_index(&stack_a);
	if (is_sort(&stack_a) == 1)
		return (0);
	sort(&stack_a, &stack_b);
	return (0);
}
