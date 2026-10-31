① DockerとCompose

 コピー
DockerはDockerfileからイメージを作り、そのイメージからコンテナを実行する仕組みです。Composeは複数のサービスやネットワーク、volumeをYAMLでまとめて管理する仕組みです。

② Composeの有無

イメージ自体は変わりません。Composeがなければdocker buildやdocker runなどを個別に管理しますが、Composeならまとめて操作できます。

③ DockerとVM

VMは独立したゲストOSとカーネルを持ちますが、Dockerコンテナはホストのカーネルを共有するので、軽量で起動が速いです。

④ ディレクトリ構成

srcs/requirements/内でサービスごとにDockerfileや設定を分離し、管理しやすくしています。Makefileはルートに配置して操作を統一しています。

---

Docker Networkは、コンテナ同士を通信させるための仮想ネットワークです。

今回はbridgeネットワークを使用しており、NGINX・WordPress・MariaDBが同じネットワークに接続されています。

各コンテナはDockerのDNSを使って、サービス名で通信できます。
bridge とは、複数のコンテナを同じ仮想ネットワークにつなぐ仕組みです。
Docker Network を使用すると、コンテナ同士が通信できます。私の Inception プロジェクトでは、NGINX、WordPress、および MariaDB は、docker-network と呼ばれるカスタムブリッジネットワークを介して接続されています。Docker は DNS 解決機能を提供するため、コンテナは IP アドレスをハードコーディングすることなく、wordpress:9000 や mariadb:3306 などのサービス名を使用して通信できます。ポート 443 をホストマシンに公開するのは NGINX のみです。

---
MariaDBはデータをコンテナ内の /var/lib/mysql に保存します。そのディレクトリをDocker named volumeにマウントしているため、コンテナを削除・再作成しても、volumeを削除しない限りデータが保持されます。