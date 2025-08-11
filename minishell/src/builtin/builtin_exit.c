/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:27:43 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 20:22:39 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static unsigned int	is_valid_exit_arg(const char *s)
{
	size_t	digit_count;

	if (!s || !*s)
		return (2);
	if (*s == '+' || *s == '-')
		s++;
	if (*s == '\0')
		return (2);
	digit_count = 0;
	while (*s)
	{
		if (!ft_isdigit(*s))
			return (2);
		digit_count++;
		s++;
	}
	if (digit_count >= 19)
		return (2);
	return (0);
}

static int	count_args(t_word_list *list)
{
	int	i;

	i = 0;
	while (list)
	{
		i++;
		list = list->next;
	}
	return (i);
}

static void	error_message(char *word)
{
	ft_dprintf(STDERR_FILENO, "minishell: exit: "
		"%s: numeric argument required\n", word);
}

int	builtin_exit(t_word_list *list, t_ctx *ctx)
{
	t_word_list	*args;
	int			arg_count;
	char		*subshell_env;
	int			exit_code;

	subshell_env = map_get(&ctx->env_map, "MINISHELL_SUBSHELL");
	if (subshell_env == NULL && isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	args = list->next;
	arg_count = count_args(args);
	if (arg_count == 0)
		exit(ctx->last_exit_code);
	exit_code = is_valid_exit_arg(args->word->word);
	if (arg_count > 1)
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		exit(2);
	}
	if (exit_code == 2)
	{
		error_message(args->word->word);
		exit(exit_code);
	}
	exit((unsigned char)ft_atoi(args->word->word));
	return (0);
}
