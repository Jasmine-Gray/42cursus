/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_helper.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 20:37:46 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "minishell.h"

static char	*handle_parent_dir(char absolute_path[PATH_MAX], char *pwd_env)
{
	char	*last_slash;

	ft_strlcpy(absolute_path, pwd_env, PATH_MAX);
	last_slash = ft_strrchr(absolute_path, '/');
	if (last_slash && last_slash != absolute_path)
		*last_slash = '\0';
	else
		ft_strlcpy(absolute_path, "/", PATH_MAX);
	return (absolute_path);
}

static char	*handle_relative_path(char absolute_path[PATH_MAX], char *pwd_env,
		char *path)
{
	ft_strlcpy(absolute_path, pwd_env, PATH_MAX);
	if (absolute_path[ft_strlen(absolute_path) - 1] != '/')
		ft_strlcat(absolute_path, "/", PATH_MAX);
	ft_strlcat(absolute_path, path, PATH_MAX);
	return (absolute_path);
}

char	*handle_deleted_dir(char *path, char absolute_path[PATH_MAX],
		t_ctx *ctx)
{
	char	*pwd_env;

	pwd_env = map_get(&ctx->env_map, "PWD");
	if (!pwd_env || pwd_env[0] != '/')
		return (path);
	if (ft_strcmp(path, "..") == 0)
		return (handle_parent_dir(absolute_path, pwd_env));
	else if (ft_strcmp(path, ".") == 0)
	{
		ft_strlcpy(absolute_path, pwd_env, PATH_MAX);
		return (absolute_path);
	}
	else if (path[0] != '/')
		return (handle_relative_path(absolute_path, pwd_env, path));
	return (path);
}

int	handle_deleted_dir_pwd(char *final_path, char absolute_path[PATH_MAX],
		t_ctx *ctx, char *old_pwd)
{
	if (final_path == absolute_path)
	{
		if (old_pwd && old_pwd[0] != '\0')
			map_set(&ctx->env_map, "OLDPWD", old_pwd);
		map_set(&ctx->env_map, "PWD", absolute_path);
		return (1);
	}
	return (0);
}

char	*prepare_path(char *path, char absolute_path[PATH_MAX], int *is_deleted,
		t_ctx *ctx)
{
	char	*final_path;
	char	tmp_cwd[PATH_MAX];

	*is_deleted = 0;
	final_path = path;
	if (path[0] == '/')
	{
		ft_strlcpy(absolute_path, path, PATH_MAX);
		final_path = absolute_path;
	}
	else
	{
		if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL && errno == ENOENT)
		{
			*is_deleted = 1;
			final_path = handle_deleted_dir(path, absolute_path, ctx);
		}
	}
	return (final_path);
}
