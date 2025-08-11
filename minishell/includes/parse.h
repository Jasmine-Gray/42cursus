/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 09:00:00 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 20:01:21 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "ast.h"
# include "minishell.h"

t_redirect	*create_redirect(t_ctx *ctx, t_word_desc *op,
				t_word_desc *filename);
void		add_redirect(t_redirect **head, t_redirect *new_redir);
int			process_all_heredocs(t_ast_node *node, t_ctx *ctx);
t_ast_node	*create_command_node(t_ctx *ctx);
t_ast_node	*create_pipe_node(t_ctx *ctx, t_ast_node *left, t_ast_node *right);
int			is_redirect_operator(int flags);
t_ast_node	*parse_command(t_ctx *ctx, t_word_list **tokens);
int			check_redirect_syntax(t_word_list **tokens);
int			process_tokens(t_ctx *ctx, t_word_list **tokens,
				t_word_list **arg_head, t_ast_node *node);
int			add_argument(t_ctx *ctx, t_word_list **arg_head,
				t_word_list **arg_tail, t_word_desc *word);
void		heredoc_sigint_handler(int sig);
int			write_to_pipe(int write_fd, const char *delimiter, t_ctx *ctx);
int			process_heredoc(const char *delimiter, int expand_vars, t_ctx *ctx);

#endif
