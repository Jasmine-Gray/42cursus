/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 22:14:23 by mishimod          #+#    #+#             */
/*   Updated: 2025/06/24 20:23:52 by mishimod         ###   ########.fr       */
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


// int main() {
//     char *home = getenv("HOME");
//     if (home != NULL) {
//         printf("HOME = %s\n", home);
//     } else {
//         printf("HOME 環境変数は設定されていません\n");
//     }
//     return 0;
// }
