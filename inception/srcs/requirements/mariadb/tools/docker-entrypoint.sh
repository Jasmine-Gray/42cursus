#!/bin/sh
set -e

# Docker secretsからパスワードを読み込む
DB_PASSWORD=$(cat /run/secrets/db_password)
DB_ROOT_PASSWORD=$(cat /run/secrets/db_root_password)

# データベースディレクトリが初期化されていない場合にセットアップを実行
if [ ! -d "/var/lib/mysql/${MYSQL_DATABASE}" ]; then

    # MariaDBの初期化
    mysql_install_db --user=mysql --datadir=/var/lib/mysql > /dev/null

    # バックグラウンドで一時的にMariaDBを起動
    mysqld_safe --datadir=/var/lib/mysql &
    
    # 起動待ち
    until mysqladmin ping --silent; do
        sleep 1
    done

    # SQLコマンドの発行（データベース・ユーザー作成・権限付与）
    mysql -e "CREATE DATABASE IF NOT EXISTS \`${MYSQL_DATABASE}\`;"
    mysql -e "CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'%' IDENTIFIED BY '${DB_PASSWORD}';"
    mysql -e "GRANT ALL PRIVILEGES ON \`${MYSQL_DATABASE}\`.* TO '${MYSQL_USER}'@'%';"
    mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${DB_ROOT_PASSWORD}';"
    mysql -e "FLUSH PRIVILEGES;"

    # 一時起動したMariaDBを停止
    mysqladmin -u root -p"${DB_ROOT_PASSWORD}" shutdown
fi

# フォアグラウンドでMariaDBを本起動（PID 1で実行し、tail -f等のハック手法を使わない）
exec mysqld --user=mysql --console
