FROM debian:bullseye

# パッケージの更新とMariaDBのインストール
RUN apt-get update -y && \
    apt-get install -y mariadb-server mariadb-client && \
    rm -rf /var/lib/apt/lists/*

# 設定ファイルとエントリポイントスクリプトのコピー
COPY conf/50-server.cnf /etc/mysql/mariadb.conf.d/50-server.cnf
COPY tools/docker-entrypoint.sh /usr/local/bin/

RUN chmod +x /usr/local/bin/docker-entrypoint.sh

# MariaDB標準ポート
EXPOSE 3306

ENTRYPOINT ["docker-entrypoint.sh"]
