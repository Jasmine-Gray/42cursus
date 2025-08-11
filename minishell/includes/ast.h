/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 05:04:59 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/12 05:05:02 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
# define AST_H

# include "minishell.h"

// AST node type enumeration
typedef enum e_node_type
{
	NODE_COMMAND,
	NODE_PIPE,
}							t_node_type;

// Forward declaration
typedef struct s_ast_node	t_ast_node;

// Pipe node content structure
typedef struct s_pipe_content
{
	t_ast_node				*left;
	t_ast_node				*right;
}							t_pipe_content;

// Redirection information structure
typedef struct s_redirect
{
	t_word_desc				*io_operator;
	t_word_desc				*filename;
	int						heredoc_fd;
	struct s_redirect		*next;
}							t_redirect;

// Command node structure
typedef struct s_node_command
{
	t_word_list				*args;
	t_redirect				*redirects;
}							t_node_command;

// AST node content union
typedef union u_ast_content
{
	t_node_command			command;
	t_pipe_content			pipe;
}							t_ast_content;

// Main AST structure
typedef struct s_ast_node
{
	t_node_type				type;
	t_ast_content			as;
}							t_ast_node;

// execute functions
int							execute_builtin(t_node_command *cmd, t_ctx *ctx);
int							do_redirections(t_redirect *redirects);
int							execute_disk_command(t_word_list *args, t_ctx *ctx);
int							get_exit_status(int status);
void						exec_right_child(int pipefd[2], t_ast_node *right,
								t_ctx *ctx);
int							execute_pipeline(t_ast_node *left,
								t_ast_node *right, t_ctx *ctx);
char						**word_list_to_argv(t_word_list *list, t_ctx *ctx);
char						*find_command_path(const char *command, t_ctx *ctx);
int							handle_redir_only(t_node_command *cmd);
void						execute_in_child(t_node_command *cmd, t_ctx *ctx);
int							wait_for_child(pid_t pid);
int							is_ambiguous_filename(t_word_desc *filename);
int							handle_input_redirect(t_redirect *redir);
int							handle_output_redirect(t_redirect *redir,
								int flags);
int							handle_heredoc_redirect(t_redirect *redir);

#endif
