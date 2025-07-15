/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 16:29:56 by mishimod          #+#    #+#             */
/*   Updated: 2025/07/15 20:28:46 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// envビルトインの実装
int	builtin_env(char **args, t_map *env_map)
{
	t_env	*current; // t_envのポインタ

	if (args[1] != NULL)
	{
		fprintf(stderr, "env: ‘%s’: No such file or directory\n", args[1]);
		return (127);
	}
	if (!env_map) // mapがNULLの場合の安全対策
		return (0);

	// 正しい代入: ポインタ(env_map->head)をポインタ(current)へ
	current = env_map->head;
	while (current)
	{
		printf("%s=%s\n", current->name, current->value);
		current = current->next;
	}
	return (0);
}


// --- 動作確認用のmain関数 ---
int	main(void)
{
	t_map *env_map = map_new();
	map_put(env_map, "PATH=/usr/bin:/bin");
	map_put(env_map, "HOME=/home/user");
	map_put(env_map, "NO_VALUE_VAR");

	printf("--- Running: env ---\n");
	char *args_ok[] = {"env", NULL};
	builtin_env(args_ok, env_map);
	printf("--------------------\n");

	map_free(env_map);
	return (0);
}
