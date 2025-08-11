# メモリリーク検出レポート

## 🔴 重大な問題が検出されました

### 1. ファイルディスクリプタリーク
**すべてのテストで FD リークが発生**
- 期待値: `FILE DESCRIPTORS: 3 open (3 std)`
- 実際: `FILE DESCRIPTORS: 4-6 open`
- **原因**: プロセス終了時にファイルディスクリプタが正しくクローズされていません

### 2. grep実行時のメモリリーク
**パイプで grep を使用した場合にリーク**
- Test 2 (ls | grep): 224 bytes definitely lost
- Test 4 (cat | grep | head): 224 bytes definitely lost
- **影響**: 外部コマンド grep 実行時のメモリ管理問題

## 詳細な検出結果

### ✅ リークなし
- 基本的なコマンド実行 (ls, pwd)
- ビルトインコマンド (echo, cd, export, env)
- 単純なリダイレクション (>, <, >>)

### ❌ FDリーク検出
**すべてのケースで発生**
- Test 1: FD 4 open (期待: 3)
- Test 2-4: FD 4-6 open (パイプ使用時に増加)
- Test 5-15: FD 4 open

### ❌ メモリリーク検出
**grepを含むパイプ実行時**
- Test 2 (ls | grep src): 224 bytes + 400 bytes indirect
- Test 4 (cat | grep | head): 224 bytes + 344 bytes indirect

## 修正が必要な箇所

### 1. execute_external.c
- fork後の子プロセスでFDのクローズ漏れ
- execve前のFDクリーンアップ不足

### 2. handle_redirects.c
- リダイレクション後のFDクローズ処理
- dup2使用後の元FDのクローズ

### 3. execute_ast.c (パイプ処理)
- パイプFDの管理
- 親プロセス・子プロセス両方でのFDクローズ

## 推奨される修正

1. **FDクローズの徹底**
   ```c
   // 子プロセスで不要なFDをクローズ
   if (pipe_fd[0] != -1)
       close(pipe_fd[0]);
   if (pipe_fd[1] != -1)
       close(pipe_fd[1]);
   ```

2. **execve前のFDクリーンアップ**
   ```c
   // execve前に標準入出力以外のFDをクローズ
   for (int fd = 3; fd < 256; fd++)
       close(fd);
   ```

3. **エラーパスでのリソース解放**
   - すべてのエラーパスでFDクローズを確認

## 緊急度: 🔴 高

これらのリークは42のnorm基準では不合格となります。
特にFDリークは深刻で、長時間実行時にリソース枯渇を引き起こす可能性があります。