/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 22:14:23 by mishimod          #+#    #+#             */
/*   Updated: 2025/06/05 16:41:57 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"
#include "stdlib.h"
#include "stdio.h"

int main()
{
		const char *name;
		name = "PATH";
		printf("%s=%s\n", name, getenv(name));
		return (0);
}

