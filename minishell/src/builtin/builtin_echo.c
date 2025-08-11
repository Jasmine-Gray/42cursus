/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 16:32:17 by mishimod          #+#    #+#             */
/*   Updated: 2025/08/09 18:50:52 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	check_n_option(const char *opt)
{
	int	i;

	if (!opt || ft_strncmp(opt, "-n", 2))
		return (false);
	i = 2;
	while (opt[i] != '\0')
	{
		if (opt[i] != 'n')
			return (false);
		i++;
	}
	return (true);
}

int	builtin_echo(t_word_list *list)
{
	bool	newline;
	bool	first;

	newline = true;
	if (list)
		list = list->next;
	while (list && check_n_option(list->word->word))
	{
		newline = false;
		list = list->next;
	}
	first = true;
	while (list)
	{
		if (!first)
			ft_dprintf(STDOUT_FILENO, " ");
		ft_dprintf(STDOUT_FILENO, "%s", list->word->word);
		first = false;
		list = list->next;
	}
	if (newline)
		ft_dprintf(STDOUT_FILENO, "\n");
	return (0);
}
