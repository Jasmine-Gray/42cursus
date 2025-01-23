/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:28:26 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/09 14:55:20 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	**ft_free(char **ptr, size_t j)
{
	while (j--)
	{
		free(ptr[j]);
	}
	free(ptr);
	return (NULL);
}

static int	count_character(char const *s, char c)
{
	size_t	i;
	int		count;

	i = 0;
	count = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		if (s[i] != '\0')
			count++;
		while (s[i] && s[i] != c)
			i++;
	}
	return (count);
}

static char	*get_word(char const *s, char c)
{
	size_t	len;
	char	*ptr;
	size_t	i;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	ptr = malloc(sizeof(char) * (len + 1));
	if (!ptr)
		return (NULL);
	i = 0;
	while (i < len)
	{
		ptr[i] = s[i];
		i++;
	}
	ptr[i] = '\0';
	return (ptr);
}

static char	**split_core(char const *s, char c, char **result)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		if (s[i])
		{
			result[j] = get_word(s + i, c);
			if (!result[j])
				return (ft_free(&result[j], j));
			j++;
			while (s[i] && s[i] != c)
				i++;
		}
	}
	result[j] = NULL;
	return (result);
}

char	**ft_split(char const *s, char c)
{
	char	**result;

	if (!s)
		return (NULL);
	result = malloc(sizeof(char *) * (count_character(s, c) + 1));
	if (!result)
		return (NULL);
	return (split_core(s, c, result));
}
