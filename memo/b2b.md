### 仮想マシンの基本的な機能
    一台のPCの中に擬似的なPCを作れる

### パスワード準拠　確認
    chage -l

### オペレーティングシステムの選択
    Debian

### RockeyとDebianの違い
    Rockeyは主に商用
    DebianのルーツはDebian
    ROckeyのルーツはRed Hat

### 仮想マシンの利点
    複数のOSを扱える
    コストの削減ができる

### aptitudeとaptの違い
    apt  apt-getを修正したもの
    aptitude 外部プロジェクトとして誕生
    apt-getが最もシンプルで、その次にaptがシンプル

### APParmorとはなにか
    セキュリティ機能の一つ
    アプリ本来の力を抑え込む
    各プログラムにセキュリティプロファイルを結びつけて、できることに制限をかける。
    プロファイルは、ネットワークアクセス、ファイルへの読み書き機能を制限できる

### 選択したオペレーティングシステム
- uname -a
- cat /etc/os-release

### サービス起動確認
    UFW  sudo ufw status
    SSH  sudo systemctl status ssh

### ユーザーの確認
    id -un

### グループの確認
    getent group sudo user42

### ユーザー作成　グループ作成
    sudo adduser [new]
    sudo groupadd [evaluating]

### グループ割当　確認
    sudo usermod -aG evaluating [new]
    id [new] -Gn

### パスワードポリシー目的　メリットデメリット
* 目的
    セキュリティの向上
* メリット
    パスワードの推測が困難になる
* デメリット
    管理者・ユーザーの手間が増える
    * 管理者　ポリシーの設定作業
    * ユーザー　ポリシー通りの設定　定期的なパスワード変更作業

### パスワードポリシー確認方法
    sudo nano /etc/pam.d /common-password

### パスワードポリシー
* minlen : 最低文字数
* lcredit : 小文字アルファベットの最低文字数
* ucredit : 大文字アルファベットの最低文字数
* dcredit : 数字の最低文字数
* maxrepeat: 同一文字の連続繰り返し最大文字数
* usercheck: ユーザーが含まれているかどうか確認
* difok: 古いパスワードとは異なる必要がある最小文字数
* enforce_for_root: rootにも同じポリシーを適用する

### 有効期限確認
    sudo nano /etc/login.defs
    sudo chage -l [username]


### ホスト名の確認と変更
    hostnamectl
    hostnamectl set-hostname [new]

### マシン再起動
    sudo reboot

### パーティションの表示
    lsblk

### LVMの仕組みとその利点
    複数のハードディスクやパーティションにまたがった記憶領域をひとつのLV(論理ボリューム)にまとめて扱うことができる。
    柔軟なストレージ管理
    動的なボリュームサイズ変更
    システムを停止せずに論理ボリュームの拡大・縮小を行える
    複数の物理ディスクを一つの論理ボリュームとして扱える

### sudoインストール確認
    sudo --version

### sudoに割当て　確認
    sudo usermod -aG sudo [new]
    id [new] -Gn

### sudoの目的
    現在ログインしているユーザーとは別のユーザーの権限でプログラムを実行するもの
    一部のプログラムを一般のユーザーに管理者権限で実行させたい場合に利用される

### sudo デモンストレーション
    nano /etc/hosts -> readonly
    sudo nano /etc/hosts

### sudo 設定実装確認
    sudo visudo

### sudo 設定内容
* secure_path パスが制限される
    sudoは実行時に環境変数PATHをsecure_pathに指定されたパスで初期化する
    設定することで、意図しないコマンドの実行を防ぐ可能性がup
* badpass_message 失敗時に指定したメッセージが表示される
* passwd_tries 3回まで
* requiretty TTYモードが有効に。これによりcron等からの実行を許可させない

    
### sudo ログ確認
    less /var/log/sudo/sodu.log

### ttyとは
    接続端末のデバイスファイル名
    tty モード有効（requiretty）
    有効にすることでcron等からの実行を許可させない


### ufw インストール確認
    sudo ufw status

### ufwとは
* FW　外部アクセスの制御を行う仕組み
      「外部からの接続は受け付けない」「sshだけ許す」のような設定ができる
      不正なアクセスを妨げる可能性がup
    UFWはFWを簡単に設定できるツール

### ポートの追加・削除
    sudo ufw allow 8080
    sudo ufw delete allow 8080

### ssh インストール確認
    sudo systemctl status ssh

### sshとは何か
    secure shellの略称
    リモートコンピューターと通信するための仕組み

### ssh設定確認
    sudo nano /etc/ssh/sshd_config

### ホスト側でユーザーIDでアクセス
    ssh [user]@localhost -p 4242


    sudo vim /usr/local/bin/monitoring.sh

    wallコマンド　すべての端末に表示
- OSの構成及びカーネルのバージョン　uname -a
- 物理プロセッサの数 /proc/cpuinfo
- 仮想プロセッサの数 /proc/cpuinfo
- サーバー上で現在使用可能なディスクとその使用率 free
- 現在のプロセッサの使用率 rop -bn1
- 前回再起動の日時 who -b
- LVMがアクティブかどうか lsblk
- アクティブな接続の数 netstat
- サーバーを使用しているユーザーの数 who
- サーバーの　IPv4アドレスとそのMACアドレス hostname -l      ip a
- sudoプログラムで実行されたコマンドの数　/var/log/sudo/sudo.log


### cronとは
    プログラムを定期的に自動実行させるための仕組み

### crontab
    sudo crontab -l
* ”分　時　日　月　曜日　実行スクリプト名”
* ”＊”は　何も指定しない
* ”\n”　のように書くとnおきに実行する

### cronの変更
    sudo crontab -u root -e

### cronの停止
    sudo systemctl stop cron
    sudo systemctl disable cron

