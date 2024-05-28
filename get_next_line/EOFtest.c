/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EOFtest.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42.jp>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 17:46:17 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/27 17:58:22 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

//int main() {
//    FILE *file = fopen("example.txt", "r");
//    if (file == NULL) {
//        printf("ファイルが開けませんでした。\n");
//        return 1;
//    }
//
//    int ch;
//    while ((ch = fgetc(file)) != EOF) {
//        putchar(ch);
//    }
//
//    fclose(file);
//    return 0;
//}

//int main() {
//    FILE *file = fopen("example.txt", "r");
//    if (file == NULL) {
//        printf("ファイルが開けませんでした。\n");
//        return 1;
//    }
//
//    int ch, lines = 0;
//    while ((ch = fgetc(file)) != EOF) {
//        if (ch == '\n') {
//            lines++;
//        }
//    }
//
//    printf("行数: %d\n", lines);
//
//    fclose(file);
//    return 0;
//}

//int main() {
//    FILE *file = fopen("example.txt", "r");
//    if (file == NULL) {
//        printf("ファイルが開けませんでした。\n");
//        return 1;
//    }
//
//    char str[100];
//    while (fgets(str, 100, file) != NULL) {
//        printf("%s", str);
//    }
//
//    fclose(file);
//    return 0;
//}

//int main() {
//    FILE *file = fopen("example.txt", "r");
//    if (file == NULL) {
//        printf("ファイルが開けませんでした。\n");
//        return 1;
//    }
//
//    int ch;
//    while ((ch = fgetc(file)) != EOF) {
//        putchar(ch);
//    }
//
//    if (ferror(file)) {
//        printf("ファイル読み込みエラーが発生しました。\n");
//    }
//
//    fclose(file);
//    return 0;
//}
