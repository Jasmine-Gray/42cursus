/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 22:12:29 by mishimod          #+#    #+#             */
/*   Updated: 2025/07/15 20:30:48 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

typedef struct s_env	t_env;
typedef struct s_map	t_map;

struct					s_env
{
	char				*name;
	char				*value;
	struct s_env		*next;
};

struct					s_map
{
	t_env *head;
};

//prototype declaration
t_env					*item_new(const char *name, const char *value);
t_map					*map_new(void);
int						map_set(t_map *map, const char *name,
							const char *value);
int						map_put(t_map *map, const char *string);
void	map_free(t_map *map); // <- map_freeの宣言
int						builtin_env(char **args, t_map *env_map);
int						get_pwd(void);
int						get_echo(const char **argv);

#endif
