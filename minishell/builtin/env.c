/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 22:14:23 by mishimod          #+#    #+#             */
/*   Updated: 2025/06/24 22:59:22 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"
#include <stdlib.h>
#include <stdio.h>

extern char **environ;

int main(void) {
    char **env = environ;
    while (*env) {
        printf("%s\n", *env); // 1つの環境変数（KEY=VALUE形式）
        env++;
    }
    return 0;
}

// int main()
// {
// 		const char *name;
// 		name = "PATH";
// 		printf("%s=%s\n", name, getenv(name));
// 		return (0);
// }


// int main() {
//     char *home = getenv("HOME");
//     if (home != NULL) {
//         printf("HOME = %s\n", home);
//     } else {
//         printf("HOME 環境変数は設定されていません\n");
//     }
//     return 0;
// }

// getenvにname(環境変数名)を渡してあげる。
// nameはenvironの挙動を実装する。
