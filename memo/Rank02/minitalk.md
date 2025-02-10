## minitalk

### server
- pid_t getpid()
    ```c
    <unistd.h>
    pid_t = プロセスIDのタイプ,符号なし整数データ型。
    自分のプロセスID を戻り値として返す。
    ```

- int pause(void)
    ```c
    <unistd.h>

    呼び出したプロセス（またはスレッド）を、シグナルを受取り終了させられるか、シグナル捕獲関数が呼び出されるまでスリープさせる。
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
- SIGUSR1 SIGUSR2
    ```c    
    アプリケーションが独自の目的で自由に利用できるシグナル。デフォルトでは特別な動作が割り当てられていない。プログラムの設計者が任意の動作を設定できる。
    ```

## bonus
argv[][]分送り終わったら、NULL文字を最後に送る。
```c
 server側がNULL文字をキャッチしたら、
 client側にシグナルを送り返す。
 ボーナスをするには sigaction() を使用して構造体で記憶しておく必要がある。
 clientに送り返すときは、signal()や kill()を使用する。
```

1. SA_SIGINFO フラグを設定する理由
```c
SA_SIGINFO フラグを設定することで、シグナルハンドラは sa_handler の代わりに sa_sigaction を使用。これにより、シグナルハンドラは追加の情報を受け取ることができる。このフラグを設定すると、シグナルハンドラ関数は以下の3つの引数を受け取ることができる。

- int signum：受信したシグナルの番号。
- siginfo_t *info：シグナルに関する追加情報（シグナルを送信したプロセスの PID など）。
- void *context：シグナルが発生したときのプロセスのコンテキスト情報
```
2. sa_handler の代わりに sa_sigaction を使用する理由
```c
sa_handler を使用すると、シグナルハンドラ関数はシグナル番号（int signum）のみを引数として受け取る。
しかし、sa_sigaction を使用すると、追加の情報（siginfo_t *info と void *context）も受け取ることができる。
具体的には、siginfo_t 構造体を通じてシグナルの発信元（シグナルを送信したプロセスの PID）などの詳細な情報を取得できる。
これにより、シグナルの処理をより柔軟かつ詳細に行うことが可能。
```

3. void *context 引数を追加する理由
```c
void *context 引数は、シグナルが発生したときのプロセスの状態（コンテキスト）を示している。
この引数は ucontext_t 構造体へのポインタであり、シグナル発生時のプロセスのレジスタやスタック情報を含んでいる。
多くの場合、この情報は必要ないが、デバッグや特定の詳細なシグナル処理を行う際には役立つことがある。
コード内では (void)context; として未使用の引数を無視している。
```

- kill(info->si_pid, SIGUSR1);
```c
if (c == 0):
    受信した文字 (c) がヌル文字 (\0) であるかどうかをチェック。

kill(info->si_pid, SIGUSR1):
    kill 関数を使用して、指定されたプロセスにシグナルを送信。
    info->si_pid は、シグナルを送信したプロセス（クライアント）のプロセスID。
    これは、siginfo_t 構造体を通じて提供される。
    SIGUSR1 は、ユーザー定義のシグナルで、ここではサーバーがクライアントにメッセージの終わりを確認するために使用される。

具体的な動作
    クライアントがサーバーにメッセージを送信し、サーバーがそのメッセージをビット単位で受信。
    サーバーが8ビット（1バイト）を受信すると、そのバイトがヌル文字 (\0) かどうかを確認。
    ヌル文字であれば、メッセージの終わりを示すため、サーバーはクライアントに SIGUSR1 シグナルを送信。
    このシグナルは、クライアントにメッセージが正常に受信されたことを通知するための確認シグナルとして機能する。
```

### Global variable
```c
グローバル変数は、異なる関数やコードのセクション間でデータや状態を共有するために使われる。例えば、シグナルハンドラやコールバック関数のように、特定のイベントが発生したときに実行される関数でデータを保持する必要がある場合に便利。

プログラムの実行中ずっと存在し続けるため、一度設定した値が失われることなく保持される。これにより、複数の関数が同じ変数にアクセスして値を読み書きすることができる。

サーバー側コード
    static int g_char_buf;
g_char_bufは、受信したビットを一時的に保持するためのバッファであり、シグナルハンドラ内で使用される。シグナルハンドラは通常、標準の関数呼び出しとは異なり、特定のイベント（シグナルの受信）によって呼び出されるため、関数の引数としてデータを渡すことができない。このため、グローバル変数を使用してデータを保持しています。

クライアント側コード
    static int g_sig_from_server;
g_sig_from_serverは、サーバーからのシグナルを一時的に保持するための変数。シグナルハンドラ内で受信したシグナルを保存し、メインの送信ループでその状態をチェックするために使用される。これにより、シグナルハンドラと送信ループ間でデータを共有することができる。
```