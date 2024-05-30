/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_test.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:54:34 by mishimod          #+#    #+#             */
/*   Updated: 2024/05/29 11:54:37 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

//#define BUF_SIZE 512
//
//int main(void)
//{
//    // 変数定義
//    int fd1, fd2;
//    int byte_num;
//    char buf[BUF_SIZE];
//
//    // 変数初期化
//    fd1 = 0;
//    fd2 = 0;
//    byte_num = 0;
//    memset(buf, '\0', BUF_SIZE); 
//
//    // ファイルのオープン
//    fd1 = open("./example.txt", O_RDONLY);  // 読み込み専用でファイルを開く
//    fd2 = open("./example.txt", O_WRONLY);  // 書き込み専用でファイルを開く
//
//    if(fd1 == -1 || fd2 == -1)  // ファイルオープン失敗
//    {
//        fprintf(stdout, "ファイルオープンエラー\n");
//        return 1;
//    }
//
//    // ファイルから5バイト読み込み
//    byte_num = read(fd1, &buf[0], 5);  // ファイルから5バイト読み込み
//
//    if(byte_num == -1)  // ファイル読み込み失敗
//    {
//        fprintf(stdout, "ファイル読み込みエラー\n");
//        return 1;
//    }
//
//    fprintf(stdout, "file1から%dバイト読み込みました。\n", byte_num);
//    fprintf(stdout, "読み込んだ文字列:%s\n", buf);
//
//    // ファイルへ3バイト書き込み
//    byte_num = write(fd2, &buf[0], 3);
//
//    if(byte_num == -1)  // ファイル書き込み失敗
//    {
//        fprintf(stdout, "ファイル書き込みエラー\n");
//        return 1;
//    }
//
//    fprintf(stdout, "file2から%dバイト書き込みました。\n", byte_num);
//    buf[byte_num] = '\0';
//    fprintf(stdout, "書き込んだ文字列:%s\n", buf);
//
//    // ファイルを閉じる
//    close(fd1);
//    close(fd2);
//
//    return 0;
//}

int main()
{
        int     fd;
        char    buff[100+1];
        int     rc;

        fd = open("example.txt", O_RDONLY);
        if (fd == -1)
        {
                printf("ファイルオープンエラー\n");
                return 1;
        }
        rc = read(fd, buff, 100);
        if (rc == -1)
        {
                printf("ファイル読み込みエラー\n");
        }
        else
        {
                buff[rc] = '\0';
                printf("read:%d - %s\n", rc, buff);
        }
        close(fd);
		return 0;
}
