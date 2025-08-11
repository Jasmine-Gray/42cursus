/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkusama <tkusama@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 00:33:41 by tkusama           #+#    #+#             */
/*   Updated: 2025/07/28 05:41:12 by tkusama          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * ft_strcmp - Compare two strings lexicographically
 *
 * Why we use unsigned char for comparison:
 *
 * Problem with signed char (8-bit):
 * - Range: -128 to 127
 * - Byte 0x80 (128 in decimal) becomes -128 in signed char
 * - This creates wrong ordering: '\x7F' (127) > '\x80' (-128)
 * - But in reality: 0x7F < 0x80 (byte values)
 *
 * Solution with unsigned char:
 * - Range: 0 to 255
 * - Byte 0x80 remains 128 (positive)
 * - Natural ordering: '\x7F' (127) < '\x80' (128)
 *
 * Real-world examples:
 * - File names with extended ASCII characters
 * - Binary data comparison
 * - Character encodings (Latin-1, CP-1252, etc.)
 *
 * POSIX standard requires unsigned char comparison for strcmp()
 */
int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	if ((int)((unsigned char)*s1 - (unsigned char)*s2) > 0)
		return (1);
	else if ((int)((unsigned char)*s1 - (unsigned char)*s2) < 0)
		return (-1);
	return (0);
}
