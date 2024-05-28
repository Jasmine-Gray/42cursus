/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 16:21:08 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/27 17:28:36 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

void	add(int num)
{
	static int	static_num;
	int			non_static_num;

	non_static_num = 0;
	static_num += num;
	non_static_num += num;
	printf("static_num: %d\n", static_num);
	printf("non_static_num: %d\n", non_static_num);
	printf("===========\n");
}

int	main(void)
{
	add(3);
	add(3);
	add(3);
	return (0);
}

__attribute__((destructor))
static void	a(void)
{
	system("leaks -q <実行ファイル名>");
}
