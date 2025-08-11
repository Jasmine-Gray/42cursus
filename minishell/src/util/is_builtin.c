/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 08:05:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/29 09:09:03 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(t_word_list *args)
{
	int		res;
	char	*cmd;

	res = 0;
	if (!args || !args->word || !args->word->word)
		return (0);
	cmd = args->word->word;
	if (ft_strcmp(cmd, "echo") == 0)
		res = 1;
	else if (ft_strcmp(cmd, "cd") == 0)
		res = 1;
	else if (ft_strcmp(cmd, "pwd") == 0)
		res = 1;
	else if (ft_strcmp(cmd, "export") == 0)
		res = 1;
	else if (ft_strcmp(cmd, "unset") == 0)
		res = 1;
	else if (ft_strcmp(cmd, "env") == 0)
		res = 1;
	else if (ft_strcmp(cmd, "exit") == 0)
		res = 1;
	else
		return (0);
	return (res);
}
