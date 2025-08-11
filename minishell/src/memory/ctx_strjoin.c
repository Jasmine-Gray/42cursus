/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx_strjoin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 18:39:52 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/27 18:46:32 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	copy_string(char *dest, const char *src, size_t offset, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		dest[offset + i] = src[i];
		i++;
	}
}

char	*ctx_strjoin(t_ctx *ctx, char const *s1, char const *s2)
{
	size_t	size_1;
	size_t	size_2;
	char	*word;

	if (!s1 || !s2)
		return (NULL);
	size_1 = ft_strlen(s1);
	size_2 = ft_strlen(s2);
	word = (char *)ctx_malloc(ctx, size_1 + size_2 + 1);
	if (!word)
		return (NULL);
	copy_string(word, s1, 0, size_1);
	copy_string(word, s2, size_1, size_2);
	word[size_1 + size_2] = '\0';
	return (word);
}
