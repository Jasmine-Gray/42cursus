FROM debian:12

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        php8.2-cli \
        php8.2-fpm \
        php8.2-mysql \
        mariadb-client \
        curl \
        wget \
        unzip && \
    rm -rf /var/lib/apt/lists/*

RUN curl -fsSL \
        https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar \
        -o /usr/local/bin/wp && \
    chmod +x /usr/local/bin/wp

COPY conf/www.conf /etc/php/8.2/fpm/pool.d/www.conf
COPY tools/docker-entrypoint.sh /usr/local/bin/docker-entrypoint.sh

RUN chmod +x /usr/local/bin/docker-entrypoint.sh && \
    mkdir -p /run/php

WORKDIR /var/www/wordpress

EXPOSE 9000

ENTRYPOINT ["docker-entrypoint.sh"]
