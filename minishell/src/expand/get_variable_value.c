/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_variable_value.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 08:31:59 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/09 08:32:10 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_variable_value(const char *name, size_t len, t_ctx *ctx)
{
	char	*var_name;
	char	*value;

	if (len == 1 && name[0] == '?')
	{
		value = ft_itoa(ctx->last_exit_code);
		return (value);
	}
	var_name = ft_strndup(name, len);
	if (!var_name)
		return (NULL);
	value = map_get(&ctx->env_map, var_name);
	free(var_name);
	if (value)
		return (ft_strdup(value));
	return (ft_strdup(""));
}
