/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libftprintf.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:45:16 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/10 17:40:36 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFTPRINTF_H
# define LIBFTPRINTF_H

# include "../libft.h"
# include <errno.h>
# include <stdarg.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>

# define HEX_CONST_LOWER "0123456789abcdef"
# define HEX_CONST_UPPER "0123456789ABCDEF"
# define BUFFER_SIZE 4096

typedef struct s_printf_ctx
{
	int			fd;
	char		buffer[BUFFER_SIZE];
	const char	*perc;
	size_t		used;
	size_t		err;
}				t_printf_ctx;

int				ft_printf(const char *format, ...);
int				ft_dprintf(int fd, const char *format, ...);
int				ft_fprintf(FILE *stream, const char *format, ...);
int				ft_vprintf(const char *format, va_list ap);
int				ft_vdprintf(int fd, const char *format, va_list ap);
int				ft_vfprintf(FILE *stream, const char *format, va_list ap);
int				len_number(long long num);
void			print_char(char c, int *count);
void			print_string(char *s, int *count);
void			print_pointer(unsigned long long p, int *count);
void			print_nbr(unsigned long long p, int *count);
void			print_nbr_x(unsigned long long p, int *count, char format);
void			print_number(int d, int *count);
void			print_unsigned_number(unsigned int d, int *count);
void			print_unsigned_hex_number(unsigned int d, int *count,
					char format);
char			*ft_itoa_unsigned(unsigned int n);
void			print_char_fd(char c, int *count, t_printf_ctx *ctx);
void			print_string_fd(char *s, int *count, t_printf_ctx *ctx);
void			print_pointer_fd(unsigned long long p, int *count,
					t_printf_ctx *ctx);
void			print_nbr_fd(unsigned long long p, int *count,
					t_printf_ctx *ctx);
void			print_nbr_x_fd(unsigned long long p, int *count, char format,
					t_printf_ctx *ctx);
void			print_number_fd(int d, int *count, t_printf_ctx *ctx);
void			print_int_fd(int d, int *count, t_printf_ctx *ctx);
void			print_unsigned_number_fd(unsigned int d, int *count,
					t_printf_ctx *ctx);
void			print_unsigned_hex_number_fd(unsigned int d, int *count,
					char format, t_printf_ctx *ctx);
void			ctx_flush(t_printf_ctx *ctx);
void			ctx_append_mem(t_printf_ctx *ctx, const void *data, size_t len);
void			ctx_append_char(t_printf_ctx *ctx, char c);
void			ctx_append_str(t_printf_ctx *ctx, const char *s);
#endif
