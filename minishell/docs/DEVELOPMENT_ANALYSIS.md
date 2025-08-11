# minishell 開発状況分析と次の対策

## 現在の実装状況

### 1. builtinコマンドの実装

現在、以下のbuiltinコマンドが実装されています：
- echo, cd, pwd, export, unset, env, exit

#### 実装の流れ：
1. `execute_ast()` (execute_ast.c:110-111)
   - `is_builtin()` で判定
   - builtinの場合 → `execute_builtin()` を呼び出し（親プロセスで実行）
   - 外部コマンドの場合 → `fork()` して子プロセスで実行

2. `execute_builtin()` (execute_builtin.c:59-73)
   - リダイレクトの処理（ファイルディスクリプタのバックアップ）
   - `search_builtin()` でコマンドを実行
   - ファイルディスクリプタの復元

### 2. パイプ処理の実装

`execute_pipe()` (execute_ast.c:137-189) で実装されています：
- 左側・右側それぞれ子プロセスを作成
- 子プロセス内で再帰的に `execute_ast()` を呼び出し

## 問題点

**builtinコマンドが子プロセスで実行されない**

現在の実装では、`execute_pipe()` の子プロセス内で `execute_ast()` を呼び出しても、
builtinコマンドの場合は親プロセスで実行されてしまいます。

```c
// execute_ast.c:163 (左側の子プロセス)
exit(execute_ast(left, ctx));  // builtinなら親プロセスで実行されてしまう

// execute_ast.c:178 (右側の子プロセス)  
exit(execute_ast(right, ctx));  // 同様の問題
```

## 解決策

### 方法1: execute_ast()に実行コンテキストを追加

```c
typedef enum e_exec_context {
    EXEC_PARENT,    // 親プロセスで実行
    EXEC_CHILD      // 子プロセスで実行（パイプ内）
} t_exec_context;

int execute_ast(t_ast_node *node, t_ctx *ctx, t_exec_context context);
```

builtinコマンドの実行時に、コンテキストに応じて処理を分岐：
- `EXEC_PARENT`: 現在の動作（親プロセスで実行）
- `EXEC_CHILD`: 子プロセス内で直接builtinを実行

### 方法2: パイプ専用のexecute関数を作成

```c
// パイプ内でのコマンド実行専用
int execute_in_pipe(t_ast_node *node, t_ctx *ctx) {
    if (node->type == NODE_COMMAND && is_builtin(node->as.command.args)) {
        // 子プロセス内で直接builtinを実行
        return search_builtin(node->as.command.args, ctx);
    }
    // その他は通常の実行
    return execute_ast(node, ctx);
}
```

### 方法3: builtinも常に子プロセスで実行

シンプルだが、`cd`や`export`など環境に影響を与えるコマンドで問題が発生。
これらは親プロセスで実行する必要があるため、非推奨。

## 推奨される実装手順

1. **方法1を採用**（より柔軟で拡張性が高い）

2. **execute_ast()の修正**
   - 第3引数として実行コンテキストを追加
   - execute_command()内でコンテキストを確認

3. **execute_pipe()の修正**
   - 子プロセス内で`execute_ast()`を呼ぶ際、`EXEC_CHILD`を指定

4. **テストケースの作成**
   ```bash
   echo hello | cat
   pwd | grep home
   ls | wc -l
   ```

5. **環境変数に影響するbuiltinの特別処理**
   - `cd`, `export`, `unset`はパイプ内でも特殊な扱いが必要
   - bashの挙動を参考に実装

## 次のステップ

1. execute_ast()にコンテキスト引数を追加
2. execute_command()でコンテキストに応じた処理を実装
3. パイプのテストを実施
4. エッジケースの処理（複数パイプ、リダイレクトとの組み合わせ）