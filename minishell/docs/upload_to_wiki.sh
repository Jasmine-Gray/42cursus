#!/bin/bash

# GitHub Wiki アップロードスクリプト
# 使い方: ./upload_to_wiki.sh

WIKI_REPO="https://github.com/yayoi111/minishell.wiki.git"
WIKI_DIR="/tmp/minishell-wiki"

echo "📚 Minishell Wiki アップロードスクリプト"
echo "========================================"

# Wiki リポジトリをクローン
echo "1. Wiki リポジトリをクローン中..."
if [ -d "$WIKI_DIR" ]; then
    rm -rf "$WIKI_DIR"
fi
git clone "$WIKI_REPO" "$WIKI_DIR" 2>/dev/null

if [ $? -ne 0 ]; then
    echo "❌ Wikiリポジトリのクローンに失敗しました"
    echo "   WikiがGitHubで有効になっているか確認してください"
    exit 1
fi

# ドキュメントをコピー
echo "2. ドキュメントをコピー中..."
cp Home.md "$WIKI_DIR/Home.md"
cp Development-History.md "$WIKI_DIR/Development-History.md"
cp Architecture.md "$WIKI_DIR/Architecture.md"
cp Testing-Strategy.md "$WIKI_DIR/Testing-Strategy.md"
cp Known-Issues.md "$WIKI_DIR/Known-Issues.md"

# サイドバーを作成
echo "3. サイドバーを作成中..."
cat > "$WIKI_DIR/_Sidebar.md" << 'EOF'
# Minishell Wiki

## 📖 Main
* [Home](Home)

## 📚 Documentation
* [Development History](Development-History)
* [Architecture](Architecture)
* [Testing Strategy](Testing-Strategy)
* [Known Issues](Known-Issues)

## 🔗 Links
* [Repository](https://github.com/yayoi111/minishell)
* [Issues](https://github.com/yayoi111/minishell/issues)
* [Pull Requests](https://github.com/yayoi111/minishell/pulls)

## 📊 Status
* Tests: 49 ✅ / 39 🚧
* Issues: 20 Open
EOF

# フッターを作成
echo "4. フッターを作成中..."
cat > "$WIKI_DIR/_Footer.md" << 'EOF'
---
*42 School Minishell Project* | [Repository](https://github.com/yayoi111/minishell) | Last Updated: $(date +%Y-%m-%d)
EOF

# Git操作
cd "$WIKI_DIR"
echo "5. Wikiを更新中..."
git add .
git commit -m "Update wiki documentation - $(date +%Y-%m-%d)"

echo ""
echo "✅ 準備完了！"
echo ""
echo "次のコマンドでWikiにプッシュしてください："
echo "  cd $WIKI_DIR"
echo "  git push"
echo ""
echo "または、このスクリプトに --push オプションを付けて実行："
echo "  ./upload_to_wiki.sh --push"

# --push オプションがある場合は自動プッシュ
if [ "$1" = "--push" ]; then
    echo ""
    echo "6. 自動プッシュ中..."
    git push
    if [ $? -eq 0 ]; then
        echo "✅ Wikiが正常に更新されました！"
        echo "🔗 https://github.com/yayoi111/minishell/wiki"
    else
        echo "❌ プッシュに失敗しました"
    fi
fi