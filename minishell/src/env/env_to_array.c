/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_to_array.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 12:20:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/27 12:20:00 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_env_entries(t_map *env_map)
{
	int		count;
	t_env	*current;

	count = 0;
	current = env_map->head;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

static char	*create_env_entry(t_ctx *ctx, t_env *env)
{
	char	*entry;
	size_t	len;

	len = ft_strlen(env->name) + ft_strlen(env->value) + 2;
	entry = ctx_malloc(ctx, len);
	if (!entry)
		return (NULL);
	ft_strlcpy(entry, env->name, len);
	ft_strlcat(entry, "=", len);
	ft_strlcat(entry, env->value, len);
	return (entry);
}

char	**get_envp_array(t_ctx *ctx)
{
	char	**envp;
	t_env	*current;
	int		i;
	int		count;

	count = count_env_entries(&ctx->env_map);
	envp = ctx_malloc(ctx, sizeof(char *) * (count + 1));
	if (!envp)
		return (NULL);
	current = ctx->env_map.head;
	i = 0;
	while (current)
	{
		envp[i] = create_env_entry(ctx, current);
		if (!envp[i])
			return (NULL);
		current = current->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}
