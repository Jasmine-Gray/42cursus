/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 08:26:32 by tkusama           #+#    #+#             */
/*   Updated: 2024/09/17 20:02:27 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(char const *s, char c)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	while (s[i] != '\0')
	{
		while (s[i] == c && s[i] != '\0')
			i++;
		if (s[i] != '\0')
			count++;
		while (s[i] != c && s[i] != '\0')
			i++;
	}
	return (count);
}

static int	word_length(char const *s, char c)
{
	int	len;

	len = 0;
	while (s[len] != '\0' && s[len] != c)
		len++;
	return (len);
}

static char	*create_word(char const *s, int len)
{
	char	*word;
	int		i;

	word = (char *)malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	i = 0;
	while (i < len)
	{
		word[i] = s[i];
		i++;
	}
	word[len] = '\0';
	return (word);
}

static char	**free_split(char **words, int n)
{
	while (n > 0)
	{
		n--;
		free(words[n]);
	}
	free(words);
	return (NULL);
}

char	**ft_split(const char *s, char c)
{
	char	**words;
	int		size;
	int		len;
	int		i;

	if (!s)
		return (NULL);
	size = count_words(s, c);
	words = (char **)malloc(sizeof(char *) * (size + 1));
	if (!words)
		return (NULL);
	i = 0;
	while (i < size)
	{
		while (*s == c && *s != '\0')
			s++;
		len = word_length(s, c);
		words[i] = create_word(s, len);
		if (!words[i])
			return (free_split(words, i));
		s += len;
		i++;
	}
	words[size] = NULL;
	return (words);
}

// #include <stdio.h>
// int main(void)
// {
//   char *str;
//   char delim;
//   char **result;

//   str = NULL;
//   delim = ' ';
//   result = ft_split(str, delim);
//   if (!result)
//     return (1);
//   for (int i = 0; result[i] != NULL; i++)
//   {
//     printf("result[%d]: %s\n", i, result[i]);
//   }
//   return (0);
// }
