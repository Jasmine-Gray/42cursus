/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 22:12:29 by mishimod          #+#    #+#             */
/*   Updated: 2025/07/15 18:50:28 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_env
{
	char			*value;
	char			*name;
	struct s_env	*next;
}					t_env;

typedef struct s_map
{
	t_env			item_head;
}					t_map;

t_env				*item_new(char *name, char *value);
t_map				*map_new(void);
char				*map_get(t_map *map, const char *name);
int					map_put(t_map *map, const char *string);
int					map_set(t_map *map, const char *name, const char *value);
int					map_unset(t_map *map, const char *name);

int					get_echo(const char **argv);
int					get_pwd(void);

#endif
