#include <stdio.h>
#include <signal.h> /* signal.hをインクルード */
#include <stdlib.h>

/* シグナルハンドラとしたい関数の定義 */
void signal_handler(int signum)
{
    /* シグナルをキャッチしたときに実行したい内容 */
    puts("中断します");
    exit(1);
}

int main(void){

    /* シグナルハンドラの登録 */
    signal(SIGINT, signal_handler);

    /* Ctl+Cで強制終了されるまで無限ループ */
    while(1){
    }

    return 0;
}

// when put ctrl+c, the signal(SIGINT) will sent to signal()function.
// Then, the signal function call "signal_handler" function.
