## minitalk

### server
- pid_t getpid()
    ```
    <unistd.h>
    pid_t = プロセスIDのタイプ,符号なし整数データ型。
    自分のプロセスID を戻り値として返す。
    ```
### client
- int kill(pid_t pid, int sig)
    ```
    <signal.h>
    プロセスにシグナルを送る。
    ```