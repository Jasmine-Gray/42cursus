#!/bin/sh
set -e

# Docker secretsからパスワードを読み込み
DB_PASSWORD=$(cat /run/secrets/db_password)
WP_ADMIN_PASSWORD=$(cat /run/secrets/credentials)

# MariaDBの起動確認（ポート3306への接続チェック）
until mysqladmin ping -h"mariadb" -u"${MYSQL_USER}" -p"${DB_PASSWORD}" --silent; do
    sleep 2
done

# WordPressが未インストールの場合は初期化を実行
if [ ! -f /var/www/wordpress/wp-config.php ]; then

    # WordPressコアファイルのダウンロード
    wp core download --allow-root

    # wp-config.php の作成
    wp config create \
        --dbname="${MYSQL_DATABASE}" \
        --dbuser="${MYSQL_USER}" \
        --dbpass="${DB_PASSWORD}" \
        --dbhost="mariadb:3306" \
        --allow-root

    # WordPressのインストールと管理者ユーザー作成（adminの文字列を含めない）
    wp core install \
        --url="https://${DOMAIN_NAME}" \
        --title="${WORDPRESS_TITLE}" \
        --admin_user="${WORDPRESS_ADMIN_USER}" \
        --admin_password="${WP_ADMIN_PASSWORD}" \
        --admin_email="${WORDPRESS_ADMIN_EMAIL}" \
        --skip-email \
        --allow-root

    # 2人目の一般ユーザー作成
    wp user create \
        "${WORDPRESS_USER}" \
        "${WORDPRESS_EMAIL}" \
        --role=author \
        --user_pass="UserPass123!" \
        --allow-root
fi

# php-fpm用実行ディレクトリの作成
mkdir -p /run/php

# フォアグラウンドでphp-fpmを起動（PID 1で実行し、infinite loop等のハック手法を使わない）
exec php-fpm7.4 -F
