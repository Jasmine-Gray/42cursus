# テスト戦略

## テストの哲学

「bashとの互換性」を最重要指標として、実際のbashの動作と比較することで正確性を保証します。

## テスト構成

```
tests/
├── test_working.sh    # ✅ 動作確認済み機能（49テスト）
├── test_todo.sh       # 🚧 修正予定の機能（39テスト）
└── (Pythonテストスイート)
```

## テストカテゴリー

### 1. Working Tests（動作確認済み）

現在100%合格率のテスト群：

#### 基本コマンド実行
```bash
# シンプルなecho
echo hello                    # ✅

# 複数引数
echo hello world test        # ✅

# 空白処理
echo hello     world        # ✅
```

#### クォート処理
```bash
# シングルクォート（変数展開なし）
echo '$PATH'                 # ✅

# ダブルクォート（変数展開あり）
echo "$USER"                 # ✅

# 混合クォート
echo "it's working"          # ✅
```

#### パイプライン
```bash
# 基本パイプ
echo hello | cat             # ✅

# 複数パイプ
echo test | cat | cat | cat  # ✅

# grepとの組み合わせ
echo hello | grep hello      # ✅
```

#### リダイレクション
```bash
# 出力リダイレクト
echo hello > file.txt        # ✅

# 入力リダイレクト
cat < file.txt               # ✅

# 追記
echo world >> file.txt       # ✅
```

### 2. TODO Tests（修正予定）

優先度別の修正予定項目：

#### 🔴 CRITICAL（クラッシュ）
```bash
# シグナルハンドリング
cat << EOF
[Ctrl+C]                     # ❌ minishell終了（Issue #31）

cat
[Ctrl+C]                     # ❌ minishell終了（Issue #32）
```

#### 🟡 HIGH（動作不良）
```bash
# 空のクォート
""                           # ❌ command not found必要（Issue #26）

# パイプチェーンエラー
a | b | c                    # ❌ ゴミ出力（Issue #33）
```

#### 🟢 MEDIUM（検証エラー）
```bash
# export検証
export 123=test              # ❌ エラー必要（Issue #30）

# exit検証
exit 999999999999999         # ❌ オーバーフロー（Issue #34）
```

## テスト実行方法

### 基本実行
```bash
# 動作確認テスト
bash tests/test_working.sh

# TODO項目確認
bash tests/test_todo.sh
```

### 個別テスト
```bash
# 特定のテストケースを実行
echo 'echo hello' | ./minishell

# bashと比較
echo 'echo hello' | bash
echo 'echo hello' | ./minishell
```

### 自動比較テスト
各テストは自動的にbashと比較：
1. 同じ入力をbashとminishellに送信
2. 出力を比較
3. 終了コードを比較
4. 差異をレポート

## テストの実装

### テスト関数の構造
```bash
test_case() {
    local test_name="$1"
    local input="$2"
    
    # bashで実行
    bash_output=$(echo "$input" | bash)
    bash_exit=$?
    
    # minishellで実行
    minishell_output=$(echo "$input" | ./minishell)
    minishell_exit=$?
    
    # 比較
    if [ "$minishell_output" = "$bash_output" ]; then
        echo "PASS"
    else
        echo "FAIL"
    fi
}
```

## テストケース設計原則

### 1. 境界値テスト
- 空入力
- 非常に長い入力
- 特殊文字

### 2. エラーケース
- 存在しないコマンド
- 権限エラー
- 構文エラー

### 3. 組み合わせテスト
- パイプ + リダイレクション
- クォート + 変数展開
- 複数のリダイレクション

## CI/CD統合（将来）

### GitHub Actions設定案
```yaml
name: Test Suite
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Compile
        run: make
      - name: Run Working Tests
        run: bash tests/test_working.sh
      - name: Check TODO Tests
        run: bash tests/test_todo.sh || true
```

## テストカバレッジ

### 現在のカバレッジ
- ✅ 基本機能: 100%
- ⚠️ エッジケース: 60%
- ❌ エラーハンドリング: 40%

### 目標カバレッジ
- 基本機能: 100%維持
- エッジケース: 80%以上
- エラーハンドリング: 70%以上

## パフォーマンステスト

### ベンチマーク項目
```bash
# 大量パイプ
time echo test | cat | cat | ... (100 pipes)

# 大容量リダイレクト
time cat large_file.txt > output.txt

# 多数のコマンド実行
time for i in {1..1000}; do echo test; done
```

## メモリリークテスト

### Valgrind使用
```bash
valgrind --leak-check=full ./minishell
```

### 確認項目
- malloc/freeの対応
- ファイルディスクリプタのクローズ
- 子プロセスの適切な終了

## 回帰テスト

### プロセス
1. バグ発見
2. バグを再現するテスト作成
3. バグ修正
4. テストが通ることを確認
5. test_working.shに追加

### 例
```bash
# Issue #26の修正後
test_case "Empty quotes fixed" '""'  # test_todo.sh → test_working.sh
```

## テストレポート

### 定期レポート生成
```bash
#!/bin/bash
echo "=== Test Report $(date) ==="
echo "Working Tests:"
bash tests/test_working.sh | tail -5
echo ""
echo "TODO Tests:"
bash tests/test_todo.sh | grep "TODO Test Summary" -A 5
```

## ベストプラクティス

### DO ✅
- bashとの比較を基準にする
- 小さなテストケースから始める
- エラーケースも必ずテスト
- テストを自動化する

### DON'T ❌
- 手動テストに依存しない
- 複雑すぎるテストケースを作らない
- テストなしでコードを変更しない

## 今後の改善点

1. **テストの並列実行** - 実行時間の短縮
2. **ビジュアルレポート** - HTMLレポート生成
3. **カバレッジ測定** - gcovによる詳細分析
4. **ファジングテスト** - ランダム入力によるクラッシュ検出
5. **ストレステスト** - 長時間実行での安定性確認