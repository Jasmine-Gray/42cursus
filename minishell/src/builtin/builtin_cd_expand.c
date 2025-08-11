/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_expand.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 08:50:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 19:32:07 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*create_expanded_path(char *home, const char *path,
		char **allocated, t_ctx *ctx)
{
	char	*expanded;
	size_t	home_len;
	size_t	path_len;

	home_len = ft_strlen(home);
	path_len = ft_strlen(path + 1);
	expanded = ctx_malloc(ctx, home_len + path_len + 1);
	if (!expanded)
	{
		ft_dprintf(STDERR_FILENO, "cd: memory allocation failed\n");
		return (NULL);
	}
	ft_strlcpy(expanded, home, home_len + 1);
	ft_strlcat(expanded, path + 1, home_len + path_len + 1);
	*allocated = expanded;
	return (expanded);
}

char	*expand_tilde(const char *path, t_ctx *ctx, char **allocated)
{
	char	*home;

	*allocated = NULL;
	if (!path || path[0] != '~')
		return ((char *)path);
	if (path[1] != '\0' && path[1] != '/')
		return ((char *)path);
	home = map_get(&ctx->env_map, "HOME");
	if (!home)
	{
		ft_dprintf(STDERR_FILENO, "cd: HOME not set\n");
		return (NULL);
	}
	if (path[1] == '\0')
		return (home);
	return (create_expanded_path(home, path, allocated, ctx));
}

char	*get_cd_path(t_word_list *list, t_ctx *ctx, char **allocated)
{
	char	*path;

	*allocated = NULL;
	if (!list->next || !list->next->word)
		return (handle_no_args(ctx));
	path = list->next->word->word;
	if (ft_strcmp(path, "-") == 0)
		return (handle_oldpwd(ctx));
	else
		return (expand_tilde(path, ctx, allocated));
}
