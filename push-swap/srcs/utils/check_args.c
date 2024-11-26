/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 18:00:50 by mishimod          #+#    #+#             */
/*   Updated: 2024/11/26 17:40:54 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

const char	**check_args(int argc, const char **argv)
{
	const char	**argv_str;

	if (argc < 2)
		return (NULL);
	else if (argc == 2)
	{
		argv_str = (const char **)ft_split(argv[1], ' ');
	}
	else
		argv_str = argv;
	return (argv_str);
}
