/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 22:12:29 by mishimod          #+#    #+#             */
/*   Updated: 2025/08/11 21:42:46 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include "libftprintf.h"
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

// struct
typedef struct s_env			t_env;
typedef struct s_map			t_map;
typedef struct s_ctx			t_ctx;
typedef struct s_word_desc		t_word_desc;
typedef struct s_word_list		t_word_list;
typedef struct s_memory_pool	t_memory_pool;
typedef struct s_ast_node		t_ast_node;

extern volatile sig_atomic_t	g_signal_number;

struct							s_memory_pool
{
	void						**ptrs;
	size_t						count;
	size_t						capacity;
};

// node
struct							s_env
{
	char						*name;
	char						*value;
	struct s_env				*next;
};

// map
struct							s_map
{
	t_env						*head;
};

// context struct (NOT global variable)
struct							s_ctx
{
	t_map						env_map;
	t_memory_pool				memory_pool;
	t_word_list					*current_cmd;
	int							last_exit_code;
	volatile sig_atomic_t		heredoc_interrupted;
};

struct							s_word_desc
{
	char						*word;
	int							flags;
};

/* A linked list of words. */
struct							s_word_list
{
	struct s_word_list			*next;
	struct s_word_desc			*word;
};

typedef struct s_token_state
{
	t_word_list					**head;
	t_word_list					**current;
}								t_token_state;

typedef enum e_word_flag
{
	WORD_NONE = 0,
	WORD_QUOTED_SINGLE = 1 << 0,
	WORD_QUOTED_DOUBLE = 1 << 1,
	WORD_EXPANDED = 1 << 2,
	WORD_ASSIGNMENT = 1 << 3,
	WORD_REDIR_IN = 1 << 4,
	WORD_REDIR_OUT = 1 << 5,
	WORD_REDIR_APPEND = 1 << 6,
	WORD_HEREDOC = 1 << 7,
	WORD_PIPE = 1 << 8,
}								t_word_flag;

typedef struct s_fd_backup
{
	int							in;
	int							out;
	int							err;
}								t_fd_backup;

typedef enum e_exec_ctx
{
	EXEC_PARENT,
	EXEC_CHILD
}								t_exec_ctx;

// map
t_env							*item_new(const char *name, const char *value);
t_map							*map_new(void);
int								map_set(t_map *map, const char *name,
									const char *value);
int								map_put(t_map *map, const char *string);
void							map_free(t_map *map);
char							*map_get(t_map *map, const char *name);
int								map_unset(t_map *map, const char *name);

// builtin func
int								builtin_echo(t_word_list *list);
int								builtin_pwd(void);
int								builtin_cd(t_word_list *list, t_ctx *ctx);

// builtin_cd_utils.c
int								bindpwd(t_ctx *ctx, const char *old_pwd);
char							*handle_no_args(t_ctx *ctx);
char							*handle_oldpwd(t_ctx *ctx);
int								cd_error(t_word_list *list);
int								change_directory(char *path, t_ctx *ctx,
									char *old_pwd);

// builtin_cd_expand.c
char							*expand_tilde(const char *path, t_ctx *ctx,
									char **allocated);
char							*get_cd_path(t_word_list *list, t_ctx *ctx,
									char **allocated);
int								builtin_export(t_word_list *list,
									t_map *env_map);
int								builtin_unset(t_word_list *list,
									t_map *env_map);
int								builtin_env(t_word_list *list, t_map *env_map);
int								builtin_exit(t_word_list *list, t_ctx *ctx);

// execute func
int								exec_builtin(t_word_list *args, t_ctx *ctx);

// tokenize
t_word_list						*tokenize(char *line, t_ctx *ctx);

// main
int								interpret(char *line, t_ctx *ctx);
void							run_repl(t_ctx *ctx);
void							fatal_error(const char *msg);

// memory
void							memory_pool_init(t_memory_pool *pool);
void							*ctx_malloc(t_ctx *ctx, size_t size);
char							*ctx_strdup(t_ctx *ctx, const char *s);
char							*ctx_strjoin(t_ctx *ctx, char const *s1,
									char const *s2);
void							ctx_free(t_ctx *ctx, void *ptr);
void							ctx_cleanup_command(t_ctx *ctx);
void							memory_pool_cleanup(t_memory_pool *pool);
void							memory_pool_remove(t_memory_pool *pool,
									void *ptr);
int								memory_pool_add(t_memory_pool *pool, void *ptr);

// signal
void							signal_handler(int sig);
void							setup_signal_handlers(void);
void							restore_original_signals(void);
void							ignore_signals_in_parent(void);
void							setup_signals_after_child(void);
int								on_rl_event(void);

// debug
void							print_tokens(t_word_list *list);
bool							is_space(char c);
int								get_flags(char *token, char quote);
char							get_quote(char c);

// tokenize
int								is_operator(char c);
char							*extract_operator(t_ctx *ctx, char **line,
									int *flags);
char							*extract_quoted_part(t_ctx *ctx, char **line,
									int *flags, char quote);
char							*extract_unquoted_part(t_ctx *ctx, char **line);
char							*extract_concatenated_word(t_ctx *ctx,
									char **line, int *flags);
void							add_word_to_list(t_ctx *ctx,
									t_token_state *state, char *word,
									int flags);

// parse
t_ast_node						*parse(t_ctx *ctx, t_word_list *tokens);

// execute
int								execute_ast(t_ast_node *node, t_ctx *ctx,
									t_exec_ctx exec_ctx);
char							**get_envp_array(t_ctx *ctx);
int								search_builtin(t_word_list *args, t_ctx *ctx);

// expand
char							*expand_variable(const char *str, t_ctx *ctx);
void							expand_word_list(t_word_list *list, t_ctx *ctx);
char							*get_variable_name(const char *str,
									size_t *len);
char							*get_variable_value(const char *name,
									size_t len, t_ctx *ctx);

// util
char							*ft_strtok(char *str, const char *delim);
char							*ft_strndup(const char *s, size_t n);
int								ft_strcmp(const char *s1, const char *s2);
int								is_builtin(t_word_list *args);
void							*ft_realloc(void *ptr, size_t new_size);

// identifier
int								is_valid_env_identifier(const char *s);
int								print_invalid_identifier(const char *cmd,
									const char *word);

#endif
