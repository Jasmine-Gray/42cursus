/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 16:32:17 by mishimod          #+#    #+#             */
/*   Updated: 2025/07/15 16:32:21 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	check_option(const char *option)
{
	int	i;

	i = 1;
	if (ft_strncmp(option, "-n", 2) == 0)
	{
		i++;
		while (option[i])
		{
			if (option[i] != 'n')
				return (false);
			i++;
		}
		return (true);
	}
	return (false);
}

int	get_echo(const char **argv)
{
	int	i;
	int	j;

	if (!argv[1])
		write(1, "\n", 1);
	if (argv[1])
	{
		i = 1;
		if (check_option(argv[1]) == true)
			i++;
		while (argv[i])
		{
			j = 0;
			while (argv[i][j])
			{
				write(1, &argv[i][j], 1);
				j++;
			}
			if (argv[i + 1])
				write(1, " ", 1);
			i++;
		}
	}
	if (check_option(argv[1]) == false)
		write(1, "\n", 1);
	return (0);
}

// int	main(int argc, const char **argv)
// {
// 	return (get_echo(argv));
// }
