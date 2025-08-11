# 42スクール評価環境を再現するDockerfile
FROM ubuntu:22.04

# タイムゾーンの設定（対話的な設定を回避）
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Tokyo

# パッケージリストの更新と基本的な開発ツールのインストール
RUN apt-get update && apt-get install -y \
    # ビルドツール
    build-essential \
    make \
    cmake \
    # デバッグツール
    gdb \
    valgrind \
    strace \
    # ライブラリ
    libreadline-dev \
    # エディタとユーティリティ
    vim \
    nano \
    git \
    curl \
    wget \
    # Python（テストスクリプト用）
    python3 \
    python3-pip \
    # その他のツール
    man \
    less \
    tree \
    htop \
    # ロケール設定用
    locales \
    && rm -rf /var/lib/apt/lists/*

# ロケールの設定
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# 作業ディレクトリの設定
WORKDIR /workspace

# 非rootユーザーの作成（42スクールの環境に近づけるため）
RUN useradd -m -s /bin/bash student && \
    echo 'student:student' | chpasswd && \
    usermod -aG sudo student

# デフォルトユーザーをstudentに設定
USER student

# bashをデフォルトシェルに設定
SHELL ["/bin/bash", "-c"]

# エントリーポイント
CMD ["/bin/bash"]