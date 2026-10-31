#!/bin/sh
set -eu

: "${MYSQL_DATABASE:?MYSQL_DATABASE is required}"
: "${MYSQL_USER:?MYSQL_USER is required}"

DB_PASSWORD=$(cat /run/secrets/db_password)
DB_ROOT_PASSWORD=$(cat /run/secrets/db_root_password)

# Prevent SQL injection through environment variables.
for identifier in "$MYSQL_DATABASE" "$MYSQL_USER"; do
    case "$identifier" in
        ''|*[!a-zA-Z0-9_]*)
            echo "Invalid database name or user" >&2
            exit 1
            ;;
    esac
done

# The project generates hexadecimal passwords using openssl.
for password in "$DB_PASSWORD" "$DB_ROOT_PASSWORD"; do
    case "$password" in
        ''|*[!0-9a-f]*)
            echo "Database passwords must be hex strings" >&2
            exit 1
            ;;
    esac
done

mkdir -p /run/mysqld /var/lib/mysql
chown -R mysql:mysql /run/mysqld /var/lib/mysql

if [ ! -f /var/lib/mysql/.inception_initialized ]; then
    # Keep plaintext SQL (which contains secrets) in a private, temporary file.
    # Do not store it in the Docker image, git repository, or persistent volume.
    init_sql=$(mktemp /run/mysqld/inception-init.XXXXXX)
    trap 'rm -f "$init_sql"' EXIT HUP INT TERM
    chown mysql:mysql "$init_sql"
    chmod 600 "$init_sql"

    cat > "$init_sql" <<SQL
FLUSH PRIVILEGES;
CREATE DATABASE IF NOT EXISTS \`${MYSQL_DATABASE}\`;
CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'%' IDENTIFIED BY '${DB_PASSWORD}';
ALTER USER '${MYSQL_USER}'@'%' IDENTIFIED BY '${DB_PASSWORD}';
GRANT ALL PRIVILEGES ON \`${MYSQL_DATABASE}\`.* TO '${MYSQL_USER}'@'%';
ALTER USER 'root'@'localhost' IDENTIFIED BY '${DB_ROOT_PASSWORD}';
FLUSH PRIVILEGES;
SQL

    if [ ! -d /var/lib/mysql/mysql ]; then
        # First initialization: mariadb-install-db runs mariadbd in --bootstrap
        # mode synchronously, then executes our extra SQL.
        mariadb-install-db \
            --user=mysql \
            --datadir=/var/lib/mysql \
            --skip-test-db \
            --extra-file="$init_sql"
    else
        # System tables exist but setup marker is absent (interrupted setup).
        # Retry the idempotent SQL without starting a background server.
        mariadbd \
            --user=mysql \
            --datadir=/var/lib/mysql \
            --bootstrap \
            --skip-networking < "$init_sql"
    fi

    # Record completion only if the initialization command succeeded.
    touch /var/lib/mysql/.inception_initialized
    chown mysql:mysql /var/lib/mysql/.inception_initialized

    rm -f "$init_sql"
    trap - EXIT HUP INT TERM
fi

# Keep MariaDB as PID 1, in the foreground.
exec mariadbd --user=mysql --console
