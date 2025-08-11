/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 08:50:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 19:31:59 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_cd(t_word_list *list, t_ctx *ctx)
{
	char	*path;
	char	*allocated_path;
	char	old_pwd[PATH_MAX];
	char	*pwd_env;
	int		ret;

	if (cd_error(list))
		return (1);
	if (getcwd(old_pwd, sizeof(old_pwd)) == NULL)
	{
		pwd_env = map_get(&ctx->env_map, "PWD");
		if (pwd_env)
			ft_strlcpy(old_pwd, pwd_env, sizeof(old_pwd));
		else
			old_pwd[0] = '\0';
	}
	path = get_cd_path(list, ctx, &allocated_path);
	if (!path)
		return (1);
	ret = change_directory(path, ctx, old_pwd);
	return (ret);
}
