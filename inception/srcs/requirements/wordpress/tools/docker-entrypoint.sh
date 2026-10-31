#!/bin/sh
set -eu

: "${MYSQL_DATABASE:?}"
: "${MYSQL_USER:?}"
: "${DOMAIN_NAME:?}"
: "${WORDPRESS_TITLE:?}"
: "${WORDPRESS_ADMIN_USER:?}"
: "${WORDPRESS_ADMIN_EMAIL:?}"
: "${WORDPRESS_USER:?}"
: "${WORDPRESS_EMAIL:?}"

DB_PASSWORD=$(cat /run/secrets/db_password)

# Make the password readable by PHP-FPM without changing host permissions
install -m 0640 -o root -g www-data /run/secrets/db_password /run/wp-db-password

# MariaDBへの接続と認証を確認する
attempts=0
until MYSQL_PWD="$DB_PASSWORD" mariadb \
    --host=mariadb \
    --user="$MYSQL_USER" \
    --database="$MYSQL_DATABASE" \
    --execute='SELECT 1' >/dev/null 2>&1; do

    attempts=$((attempts + 1))

    if [ "$attempts" -ge 60 ]; then
        echo "MariaDB did not become ready" >&2
        exit 1
    fi

    sleep 2
done

cd /var/www/wordpress

# WordPressの本体ファイルを配置
if [ ! -f wp-settings.php ]; then
    wp core download --allow-root --force
fi

# WordPressのDB接続設定を作成
if [ ! -f wp-config.php ]; then
    wp config create \
        --dbname="$MYSQL_DATABASE" \
        --dbuser="$MYSQL_USER" \
        --dbhost=mariadb:3306 \
        --allow-root \
        --dbpass=placeholder --skip-check --quiet
fi

# Apply the same secret-based configuration on every startup
wp config set DB_PASSWORD 'trim(file_get_contents("/run/wp-db-password"))' --raw --allow-root --quiet

# WordPressがDBにインストールされていなければ実行
if ! wp core is-installed --allow-root >/dev/null 2>&1; then
    wp core install \
        --url="https://${DOMAIN_NAME}" \
        --title="$WORDPRESS_TITLE" \
        --admin_user="$WORDPRESS_ADMIN_USER" \
        --admin_email="$WORDPRESS_ADMIN_EMAIL" \
        --skip-email \
        --allow-root \
        --prompt=admin_password < /run/secrets/credentials >/dev/null 2>&1
fi

# 2人目のユーザーを作成
if ! wp user get "$WORDPRESS_USER" --allow-root >/dev/null 2>&1; then
    wp user create \
        "$WORDPRESS_USER" "$WORDPRESS_EMAIL" \
        --role=author \
        --allow-root \
        --prompt=user_pass < /run/secrets/wp_user_password >/dev/null 2>&1
fi

chown -R www-data:www-data /var/www/wordpress

mkdir -p /run/php

# PHP-FPMをフォアグラウンドで起動
exec php-fpm8.2 -F
