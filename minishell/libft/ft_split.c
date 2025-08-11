/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 05:39:26 by tkusama           #+#    #+#             */
/*   Updated: 2025/08/11 05:39:28 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(char const *str, char delim);
static int	word_length(char const *str, char delim);
static char	*create_word(char const *str, int len);
static char	**free_split(char **result, int count);

char	**ft_split(const char *str, char delim)
{
	char	**result;
	int		count;
	int		len;
	int		i;

	if (!str)
		return (NULL);
	count = count_words(str, delim);
	result = malloc(sizeof(char *) * (count + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (i < count)
	{
		while (*str == delim)
			str++;
		len = word_length(str, delim);
		result[i] = create_word(str, len);
		if (!result[i])
			return (free_split(result, i));
		str += len;
		i++;
	}
	result[count] = NULL;
	return (result);
}

static int	count_words(char const *str, char delim)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	while (str[i] != '\0')
	{
		while (str[i] == delim && str[i] != '\0')
			i++;
		if (str[i] != '\0')
			count++;
		while (str[i] != delim && str[i] != '\0')
			i++;
	}
	return (count);
}

static int	word_length(char const *str, char delim)
{
	int	len;

	len = 0;
	while (str[len] != '\0' && str[len] != delim)
		len++;
	return (len);
}

static char	*create_word(char const *str, int len)
{
	char	*word;

	word = malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	ft_memcpy(word, str, len);
	word[len] = '\0';
	return (word);
}

static char	**free_split(char **result, int count)
{
	while (count-- > 0)
		free(result[count]);
	free(result);
	return (NULL);
}
