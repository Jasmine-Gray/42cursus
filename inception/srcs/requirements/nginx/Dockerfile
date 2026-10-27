FROM debian:bullseye

# パッケージ更新とNGINX、OpenSSLのインストール
RUN apt-get update -y && \
    apt-get install -y nginx openssl && \
    rm -rf /var/lib/apt/lists/*

# SSL自己署名証明書および秘密鍵の生成
RUN mkdir -p /etc/nginx/ssl && \
    openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
    -keyout /etc/nginx/ssl/inception.key \
    -out /etc/nginx/ssl/inception.crt \
    -subj "/C=JP/ST=Tokyo/L=Tokyo/O=42School/OU=42Tokyo/CN=mishimod.42.fr"

# 設定ファイルのコピー
COPY conf/nginx.conf /etc/nginx/conf.d/default.conf

# 外部公開ポート（TLSポートのみ）
EXPOSE 443

# フォアグラウンドでNGINXを起動（infinite loop等のハック手法を使わない）
CMD ["nginx", "-g", "daemon off;"]
