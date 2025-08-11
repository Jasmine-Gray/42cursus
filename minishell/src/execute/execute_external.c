/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_external.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 16:42:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/10 16:42:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

char	**word_list_to_argv(t_word_list *list, t_ctx *ctx);
char	*find_command_path(const char *command, t_ctx *ctx);

int	execute_disk_command(t_word_list *args, t_ctx *ctx)
{
	char		**argv;
	char		*cmd_path;
	char		**envp;
	struct stat	st;

	argv = word_list_to_argv(args, ctx);
	if (!argv)
		return (127);
	cmd_path = find_command_path(argv[0], ctx);
	if (!cmd_path)
	{
		ft_dprintf(STDERR_FILENO, "minishell: %s: command not found\n",
			argv[0]);
		return (127);
	}
	if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_dprintf(STDERR_FILENO, "minishell: %s: is a directory\n", argv[0]);
		return (126);
	}
	envp = get_envp_array(ctx);
	execve(cmd_path, argv, envp);
	perror("execve");
	return (126);
}
