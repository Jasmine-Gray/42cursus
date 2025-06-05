/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 22:14:23 by mishimod          #+#    #+#             */
/*   Updated: 2025/06/05 17:27:31 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"
#include "stdlib.h"
#include "stdio.h"

// int main()
// {
// 		const char *name;
// 		name = "PATH";
// 		printf("%s=%s\n", name, getenv(name));
// 		return (0);
// }

int main(int argc, char *argv[], char *envp[])
{
    int i = 0;

    /*  argc の確認 */
    printf("argc = %d\n", argc);

    /* argv の確認 */
    printf("argv = [");
    for (i = 0; i < argc; i++)
    {
        printf("\'%s\' ", argv[i]);
    }
    printf("]\n");

    /* envp の確認 */
    printf("envp = [");
    for (i = 0; envp[i]; i++)
    {
        printf("\'%s\' \n", envp[i]);
    }
    printf("]\n");

    return 0;
}
