/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_external_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 00:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 00:00:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

char	**word_list_to_argv(t_word_list *list, t_ctx *ctx)
{
	char		**argv;
	int			count;
	int			i;
	t_word_list	*current;

	count = 0;
	current = list;
	while (current)
	{
		count++;
		current = current->next;
	}
	argv = ctx_malloc(ctx, sizeof(char *) * (count + 1));
	if (!argv)
		return (NULL);
	i = 0;
	current = list;
	while (current)
	{
		argv[i] = current->word->word;
		i++;
		current = current->next;
	}
	argv[i] = NULL;
	return (argv);
}

static char	*check_direct_path(const char *command, t_ctx *ctx)
{
	if (ft_strchr(command, '/'))
	{
		if (access(command, F_OK) == 0)
		{
			if (access(command, X_OK) == 0)
				return (ctx_strdup(ctx, command));
		}
		return (ctx_strdup(ctx, command));
	}
	return (NULL);
}

static char	*build_full_path(const char *dir, const char *cmd,
	size_t cmd_len, t_ctx *ctx)
{
	char	*full_path;
	size_t	dir_len;

	dir_len = ft_strlen(dir);
	full_path = ctx_malloc(ctx, dir_len + cmd_len + 2);
	if (!full_path)
		return (NULL);
	ft_strlcpy(full_path, dir, dir_len + 1);
	full_path[dir_len] = '/';
	ft_strlcpy(full_path + dir_len + 1, cmd, cmd_len + 1);
	return (full_path);
}

static char	*search_in_path(const char *command, char *path_copy, t_ctx *ctx)
{
	char	*dir;
	char	*full_path;
	size_t	cmd_len;

	cmd_len = ft_strlen(command);
	dir = ft_strtok(path_copy, ":");
	while (dir)
	{
		full_path = build_full_path(dir, command, cmd_len, ctx);
		if (!full_path)
			return (NULL);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		ctx_free(ctx, full_path);
		dir = ft_strtok(NULL, ":");
	}
	return (NULL);
}

char	*find_command_path(const char *command, t_ctx *ctx)
{
	char	*path_env;
	char	*path_copy;
	char	*result;

	result = check_direct_path(command, ctx);
	if (result)
		return (result);
	path_env = map_get(&ctx->env_map, "PATH");
	if (!path_env)
		return (NULL);
	path_copy = ctx_strdup(ctx, path_env);
	if (!path_copy)
		return (NULL);
	return (search_in_path(command, path_copy, ctx));
}
