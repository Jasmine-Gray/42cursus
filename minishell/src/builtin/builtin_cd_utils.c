/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 08:50:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 15:31:34 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	bindpwd(t_ctx *ctx, const char *old_pwd)
{
	char	new_pwd[PATH_MAX];

	if (getcwd(new_pwd, sizeof(new_pwd)) == NULL)
	{
		if (errno == ENOENT)
		{
			if (old_pwd && old_pwd[0] != '\0')
				map_set(&ctx->env_map, "OLDPWD", old_pwd);
			return (0);
		}
		perror("cd");
		return (1);
	}
	if (old_pwd && old_pwd[0] != '\0')
		map_set(&ctx->env_map, "OLDPWD", old_pwd);
	map_set(&ctx->env_map, "PWD", new_pwd);
	return (0);
}

char	*handle_no_args(t_ctx *ctx)
{
	char	*home;

	home = map_get(&ctx->env_map, "HOME");
	if (!home)
	{
		ft_dprintf(STDERR_FILENO, "cd: HOME not set\n");
		return (NULL);
	}
	return (home);
}

char	*handle_oldpwd(t_ctx *ctx)
{
	char	*path;

	path = map_get(&ctx->env_map, "OLDPWD");
	if (!path)
	{
		ft_dprintf(STDERR_FILENO, "cd: OLDPWD not set\n");
		return (NULL);
	}
	ft_printf("%s\n", path);
	return (path);
}

int	cd_error(t_word_list *list)
{
	if (list->next && list->next->next)
	{
		ft_dprintf(STDERR_FILENO, "cd: too many arguments\n");
		return (1);
	}
	return (0);
}

char	*handle_deleted_dir(char *path, char absolute_path[PATH_MAX],
		t_ctx *ctx);
int		handle_deleted_dir_pwd(char *final_path, char absolute_path[PATH_MAX],
		t_ctx *ctx, char *old_pwd);
char	*prepare_path(char *path, char absolute_path[PATH_MAX],
		int *is_deleted, t_ctx *ctx);

int	change_directory(char *path, t_ctx *ctx, char *old_pwd)
{
	char	absolute_path[PATH_MAX];
	char	*final_path;
	int		is_deleted_dir;

	final_path = prepare_path(path, absolute_path, &is_deleted_dir, ctx);
	if (chdir(final_path) != 0)
	{
		ft_dprintf(STDERR_FILENO, "cd: %s: %s\n", path, strerror(errno));
		return (1);
	}
	if (is_deleted_dir)
	{
		if (handle_deleted_dir_pwd(final_path, absolute_path, ctx, old_pwd))
			return (0);
	}
	return (bindpwd(ctx, old_pwd));
}
