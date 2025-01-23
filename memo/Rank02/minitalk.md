## minitalk

### server
- pid_t getpid()
    ```c
    <unistd.h>
    pid_t = プロセスIDのタイプ,符号なし整数データ型。
    自分のプロセスID を戻り値として返す。
    ```
### client
- int kill(pid_t pid, int sig)
    ```c
    <signal.h>
    プロセスにシグナルを送る。
    
    - 返り値 -
    正の値 シグナル sig が pid で指定された ID を持つプロセスに送られる。
    0 呼び出し元のプロセスのプロセスグループに属するすべてのプロセスに sig で指定したシグナルが送られる。
    -1 | sig で指定したシグナルが、呼び出し元のプロセスがシグナルを送る許可を持つ全てのプロセスに 送られる。 但し、プロセス番号 1 (init) へはシグナルは送られない。
    -1より小さい値 | ID が -pid のプロセスグループに属するすべてのプロセスに sig で指定したシグナルが送られる。
    ```
- int sigemptyset(sigset_t *set)
    ```c
    <signal.h>
    シグナル・セットsetを、から集合に初期設定する。
    認識されたシグナルはすべて除外される。

    シグナル・セット->
        シグナル・グループを一つのプロセスにより、経過の記録を取るためのデータ・オブジェクト

    sigemptyset(&sa.sa_mask)  でsaのシグナルマスクをクリアする。
    
    シグナルマスク->
    「○○のシグナルハンドラの実行中は、××のシグナルは無視する」というときに便利
    シグナルは、イベントが起こるとプログラムの実行部分に関係なく割り込んでくるため、それを防ぐことができる。
    sigemptyset()でシグナルマスクをゼロクリアできる。

    sigemptyset(&sa.sa_mask);
    ```
- unsigned int sleep(unsigned int seconds)
    ```c
    指定の秒数の間だけ休止する。
    ```

- sigaction 
    ```c
    sigactionを使うときは構造体が必要。

    struct sigaction {
    void     (*sa_handler)(int);
    void     (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t   sa_mask;
    int        sa_flags;
    void     (*sa_restorer)(void);
    };

    struct sigaction sa;
    ```
    ```c
    sigactionを使用すれば、シグナルをトリガーにして、関数を起動できる。
    sigactionは一度動かすとプロセス終了までずっと設定したシグナルをキャッチしてくれる。

    void	receiver(void handler(int, siginfo_t *, void *))
    {
    	struct sigaction	act;
    
    	bzero(&act, sizeof(struct sigaction));
    	act.sa_sigaction = handler;
    	sigemptyset(&act.sa_mask);
    	act.sa_flags = SA_SIGINFO;
    	sigaction(SIGUSR1, &act, NULL);
    	sigaction(SIGUSR2, &act, NULL);
    }
    
    bzeroで構造体の初期化
    sigemptysetでシグナルの初期化
    act.sa_flags = SA_SIGINFO;でhandler関数を使用する設定にする。
    sigactionは第一引数にどのシグナルをトリガーにするか設定。
    複数のシグナルで同じ関数を動かすときには、上記のように複数回呼び出す。
    ```
    ```c
    連続してシグナルを送る場合のインターバルの設定
    文字列をbitに変換し、プロセスに送り、連続して何度もシグナルを送る。

    void	send_char(pid_t pid, char c)
    {
    	int				bit;
    	int				i;
    	unsigned char	uc;

    	uc = (unsigned char)c;
    	i = 0;
    	while (i < 8)
    	{
    		usleep(50);
    		bit = (uc >> i) & 0x01;
    		if (kill(pid, SIGUSR1 + bit) == -1)
    			fatal("kill error");
    		i++;
    	}
    }
    ```
    