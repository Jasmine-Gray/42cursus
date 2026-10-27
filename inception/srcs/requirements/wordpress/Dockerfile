FROM debian:bullseye

# PHPおよび必要な拡張機能、wp-cliのダウンロード用パッケージをインストール
RUN apt-get update -y && \
    apt-get install -y \
    php7.4 \
    php7.4-fpm \
    php7.4-mysql \
    mariadb-client \
    curl \
    wget \
    unzip && \
    rm -rf /var/lib/apt/lists/*

# wp-cli (WordPress Command Line Interface) のインストール
RUN curl -O https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar && \
    chmod +x wp-cli.phar && \
    mv wp-cli.phar /usr/local/bin/wp

# php-fpmの設定ファイルをコピー
COPY conf/www.conf /etc/php/7.4/fpm/pool.d/www.conf
COPY tools/docker-entrypoint.sh /usr/local/bin/

RUN chmod +x /usr/local/bin/docker-entrypoint.sh

# FastCGIポート
EXPOSE 9000

WORKDIR /var/www/wordpress

ENTRYPOINT ["docker-entrypoint.sh"]
