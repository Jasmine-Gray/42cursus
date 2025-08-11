# アーキテクチャと設計思想

## 全体設計

Minishellは、入力から実行まで複数の段階を経て処理を行うパイプライン型アーキテクチャを採用しています。

```
┌─────────┐    ┌────────┐    ┌────────┐    ┌──────────┐    ┌──────────┐
│  Input  │───>│ Lexer  │───>│ Parser │───>│ Expander │───>│ Executor │
└─────────┘    └────────┘    └────────┘    └──────────┘    └──────────┘
     ↓              ↓             ↓              ↓               ↓
  ユーザー      トークン      構文木       変数展開        コマンド
   入力          生成         構築          処理           実行
```

## コンポーネント詳細

### 1. Lexer（字句解析器）

**役割**: 文字列をトークンに分解

**主要ファイル**:
- `src/lexer/lexer.c`
- `src/lexer/lexer_utils.c`

**トークンタイプ**:
```c
typedef enum {
    TOKEN_WORD,      // 通常の単語
    TOKEN_PIPE,      // |
    TOKEN_REDIR_IN,  // <
    TOKEN_REDIR_OUT, // >
    TOKEN_APPEND,    // >>
    TOKEN_HEREDOC,   // <<
    TOKEN_EOF        // 終端
} t_token_type;
```

**設計原則**:
- 状態機械による実装
- クォート処理の正確な実装
- 特殊文字のエスケープ処理

### 2. Parser（構文解析器）

**役割**: トークンから抽象構文木（AST）を構築

**主要ファイル**:
- `src/parser/parser.c`
- `src/parser/parser_utils.c`

**ASTノードタイプ**:
```c
typedef enum {
    AST_COMMAND,     // コマンド実行
    AST_PIPE,        // パイプ
    AST_REDIRECT,    // リダイレクション
    AST_HEREDOC      // ヒアドキュメント
} t_ast_type;
```

**構文規則**:
```
pipeline    : command
            | pipeline '|' command
            ;

command     : simple_cmd
            | simple_cmd redirections
            ;

simple_cmd  : word
            | simple_cmd word
            ;

redirections: redirection
            | redirections redirection
            ;
```

### 3. Expander（展開器）

**役割**: 変数展開とパス解決

**主要ファイル**:
- `src/executor/expander.c`

**展開処理**:
1. 環境変数展開（`$VAR`）
2. 特殊変数（`$?` - 終了ステータス）
3. クォート内での展開制御

**展開ルール**:
- ダブルクォート内: 変数展開あり
- シングルクォート内: 変数展開なし
- クォート外: 変数展開あり

### 4. Executor（実行器）

**役割**: ASTを解釈してコマンドを実行

**主要ファイル**:
- `src/executor/executor.c`
- `src/executor/execute_command.c`
- `src/executor/execute_pipeline.c`

**実行フロー**:
```
1. ASTノードのタイプを判定
2. ビルトインコマンドチェック
3. fork()による子プロセス生成
4. execve()による外部コマンド実行
5. wait()による終了待機
```

### 5. Builtin Commands（ビルトインコマンド）

**役割**: シェル内部で実行されるコマンド

**実装コマンド**:
```
builtin/
├── builtin_cd.c      # ディレクトリ変更
├── builtin_echo.c    # テキスト出力
├── builtin_env.c     # 環境変数表示
├── builtin_exit.c    # シェル終了
├── builtin_export.c  # 環境変数設定
├── builtin_pwd.c     # カレントディレクトリ
└── builtin_unset.c   # 環境変数削除
```

**ビルトインが必要な理由**:
- `cd`: 親プロセスの環境を変更する必要がある
- `export/unset`: 環境変数の操作
- `exit`: シェル自体の終了

## データ構造

### 環境変数管理
```c
typedef struct s_env {
    char            *key;
    char            *value;
    struct s_env    *next;
} t_env;
```

### パイプライン管理
```c
typedef struct s_pipe {
    int     pipe_fd[2];  // パイプのファイルディスクリプタ
    pid_t   pid;         // 子プロセスのPID
} t_pipe;
```

### リダイレクション管理
```c
typedef struct s_redirect {
    int         type;    // <, >, >>
    char        *file;   // ファイル名
    int         fd;      // ファイルディスクリプタ
} t_redirect;
```

## エラーハンドリング戦略

### 1. メモリ管理
- すべてのmalloc()をチェック
- 適切なfree()による解放
- Valgrindでのリークチェック

### 2. プロセス管理
- fork()失敗時の処理
- 子プロセスの適切な終了
- ゾンビプロセスの防止

### 3. ファイルディスクリプタ
- 使用後の確実なclose()
- dup2()による適切な複製
- リークの防止

### 4. シグナル処理
```c
// Ctrl+C (SIGINT)
signal(SIGINT, sigint_handler);

// Ctrl+\ (SIGQUIT)
signal(SIGQUIT, SIG_IGN);
```

## パフォーマンス考慮事項

### 最適化ポイント
1. **不要なfork()の回避**: ビルトインコマンドは親プロセスで実行
2. **バッファリング**: 入出力の効率化
3. **メモリプール**: 頻繁なmalloc/freeの削減（将来的な改善）

### ボトルネック
- パイプの大量連結時のプロセス生成
- 大きなファイルのリダイレクション
- 環境変数の線形探索

## セキュリティ考慮事項

### 入力検証
- バッファオーバーフローの防止
- インジェクション攻撃の防止
- パス遍歴の制限

### 権限管理
- setuid/setgidビットの適切な処理
- ファイルパーミッションの確認

## 設計パターン

### 1. Chain of Responsibility
Lexer → Parser → Expander → Executorの処理チェーン

### 2. Interpreter Pattern
ASTノードの解釈実行

### 3. Strategy Pattern
ビルトインコマンドと外部コマンドの実行戦略切り替え

## 将来の拡張性

### モジュール化
各コンポーネントが独立しているため、機能追加が容易：
- 新しいビルトインコマンドの追加
- 新しいリダイレクションタイプ
- 追加の演算子（&&, ||）

### テスタビリティ
各段階が分離されているため、単体テストが可能：
- Lexerのトークン化テスト
- Parserの構文解析テスト
- Executorの実行テスト

## 制限事項と妥協点

### 意図的な簡略化
1. グロブ展開なし（`*.txt`）
2. コマンド置換なし（`$(...)`)
3. 算術展開なし（`$((...))`

### 技術的制限
1. パイプのネスト深度制限
2. 環境変数サイズの制限
3. コマンドライン長の制限

これらの制限は、プロジェクトの範囲を適切に保ち、基本機能の確実な実装に注力するための意図的な決定です。