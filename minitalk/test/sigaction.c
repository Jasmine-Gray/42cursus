#include <stdio.h>
#include <signal.h> /* signal.hをインクルード */
#include <stdlib.h>
#include <string.h>

int count;

/* シグナルハンドラとしたい関数の定義 */
void signal_handler(int signum)
{
    /* シグナルハンドラ内で安全ではない関数puts()を使っている */
    puts("中断します");
    exit(1);
}

int main(void){

    /* シグナルハンドラの登録 */
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

	/* Ctl+Cで強制終了されるまで無限ループ */
    while(1){
    }

    return 0;
}
