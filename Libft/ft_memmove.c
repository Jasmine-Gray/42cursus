/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 19:13:31 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/08 16:34:31 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char		*d;
	unsigned const char	*s;

	d = (unsigned char *)dst;
	s = (unsigned const char *)src;
	if (!dst && !src)
		return (NULL);
	if (d < s)
		while (len--)
			*d++ = *s++;
	else
		while (len--)
			d[len] = s[len];
	return (dst);
}

// #define STR_SIZE 42
//  void	input_test_case(char src_1[STR_SIZE][STR_SIZE],
//  		char src_2[STR_SIZE][STR_SIZE])
//  {
//  	strcpy(src_1[0], "");
//  	strcpy(src_2[0], "");
//  	strcpy(src_1[1], "hello world");
//  	strcpy(src_2[1], "hello world");
//  	strcpy(src_1[2], "test");
//  	strcpy(src_2[2], "test");
//  	strcpy(src_1[3], "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
//  	strcpy(src_2[3], "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
//  }
//  bool	check_dst_befor_src(char *src_1, char *src_2)
//  {
//  	printf("Before: src_1[%p]: %s\n", src_1, src_1);
//  	printf("Before: src_2[%p]: %s\n", src_2, src_2);
//  	ft_memmove(src_1, src_1 + 3, STR_SIZE / 2);
//  	memmove(src_2, src_2 + 3, STR_SIZE / 2);
//  	printf("After: src_1[%p]: %s\n", src_1, src_1 + 3);
//  	printf("After: src_2[%p]: %s\n", src_2, src_2 + 3);
//  	return (true);
//  }
//
//  bool	check_dst_in_src(char *src_1, char *src_2)
//  {
//  	printf("Before: src_1[%p]: %s\n", src_1, src_1);
//  	printf("Before: src_2[%p]: %s\n", src_2, src_2);
//  	ft_memmove(src_1 + 3, src_1, STR_SIZE / 2);
//  	memmove(src_2 + 3, src_2, STR_SIZE / 2);
//  	printf("After: src_1[%p]: %s\n", src_1 + 3, src_1);
//  	printf("After: src_2[%p]: %s\n", src_2 + 3, src_2);
//  	return (true);
//  }
//
//  bool	check_dst_after_src(char *dst_1, char *dst_2, char *src_1)
//  {
//  	printf("Before: src_1[%p]: %s\n", dst_1, src_1);
//  	printf("Before: src_2[%p]: %s\n", dst_2, src_1);
//  	ft_memmove(dst_1, src_1, STR_SIZE);
//  	memmove(dst_2, src_1, STR_SIZE);
//  	printf("After: src_1[%p]: %s\n", dst_1, src_1);
//  	printf("After: src_2[%p]: %s\n", dst_2, src_1);
//  	return (true);
//  }
//
//  bool	check_each_charactor(const char **src_1, const char **src_2)
//  {
//  	int	i;
//  	int	j;
//
//  	i = 0;
//  	while (i < STR_SIZE)
//  	{
//  		j = 0;
//  		while (j < STR_SIZE)
//  		{
//  			if (src_1[i][j] != src_2[i][j])
//  			{
//  				printf("ft_memmove=%s, memmove=%s\n", src_1[i], src_2[i]);
//  			}
//  			else if (src_1[i + 3][j] != src_2[i + 3][j])
//  			{
//  				printf("ft_memmove=%s, memmove=%s\n", src_1[i + 3], src_2[i
//  					+ 3]);
//  			}
//  			else if (dst_1[i][j] != dst_2[i][j])
//  			{
//  				printf("ft_memmove=%s, memmove=%s\n", dst_1[i], dst_2[i]);
//  			}
//  			return (false);
//  			j++;
//  		}
//  		i++;
//  	}
//  	return (true);
//  }
//
//  bool	check_memmove(void)
//  {
//  	char	dst_1[STR_SIZE][STR_SIZE];
//  	char	dst_2[STR_SIZE][STR_SIZE];
//  	char	src_1[STR_SIZE][STR_SIZE];
//  	char	src_2[STR_SIZE][STR_SIZE];
//  	int		i;
//  	int		j;
//
//  	i = 0;
//  	while (i < STR_SIZE)
//  	{
//  		j = 0;
//  		while (j < STR_SIZE)
//  		{
//  			src_1[i][j] = '\0';
//  			src_2[i][j] = '\0';
//  			j++;
//  		}
//  		i++;
//  	}
//  	input_test_case(**src_1, **src_2);
//  	i = 0;
//  	while (i < STR_SIZE)
//  	{
//  		if (!check_dst_befor_src(src_1[i], src_2[i]))
//  		{
//  			return (false);
//  		}
//  		if (!check_dst_in_src())
//  		{
//  			return (false);
//  		}
//  		if (!check_dst_after_src())
//  		{
//  			return (false);
//  		}
//  		i++;
//  	}
//  	return (true);
//  }
//
//  int	main(void)
//  {
//  	if (check_memmove())
//  	{
//  		printf("OK\n");
//  	}
//  	else
//  	{
//  		printf("ERROR\n");
//  	}
//  }
//
