# AST (Abstract Syntax Tree) 詳細解説

## ASTとは何か？

AST（抽象構文木）は、プログラムやコマンドの構造を木構造で表現したデータ構造です。minishellでは、シェルコマンドの論理的な構造を表現するために使用されています。

## なぜASTが必要なのか？

### 1. 文字列からの脱却
```bash
# 元の文字列
"ls -la | grep test > output.txt"

# トークン化後（まだ線形）
["ls", "-la", "|", "grep", "test", ">", "output.txt"]

# AST化後（構造化）
パイプ
├── コマンド: ls -la
└── コマンド: grep test
    └── リダイレクト: > output.txt
```

### 2. 実行順序の明確化
- 文字列やトークンリストでは、実行順序や依存関係が不明確
- ASTでは、親子関係により実行順序が明確

### 3. 再帰的な処理の簡潔化
- 木構造は再帰的な処理に適している
- 各ノードタイプに応じた処理を定義するだけで、複雑な構造も処理可能

## minishellのAST構造詳細

### ノードの基本構造
```c
struct s_ast_node {
    t_node_type type;  // ノードの種類
    union {
        t_node_command command;  // コマンドノードの場合
        struct {
            t_ast_node *left;    // パイプの左側
            t_ast_node *right;   // パイプの右側
        } pipe;
    } as;
};
```

### Union（共用体）の利点
- メモリ効率：各ノードタイプに必要なデータのみを保持
- 型安全性：ノードタイプに応じて適切なフィールドにアクセス

## AST構築の詳細プロセス

### 1. トークンストリームの消費
```c
// parse_simple_command内
while (*tokens && !((*tokens)->word->flags & WORD_PIPE)) {
    // トークンを一つずつ消費
    *tokens = (*tokens)->next;
}
```
- ポインタのポインタ（`**tokens`）により、呼び出し元のトークン位置も更新

### 2. ボトムアップ構築
```
Step 1: 各コマンドをパース（葉ノード）
Step 2: パイプで接続（内部ノード）
Step 3: さらにパイプがあれば再帰的に接続
```

### 3. 左結合の実装
```c
// parse関数内
left = parse_simple_command(ctx, &tokens);
while (tokens && (tokens->word->flags & WORD_PIPE)) {
    right = parse_simple_command(ctx, &tokens);
    left = create_pipe_node(ctx, left, right);  // 左に積み上げる
}
```

## 複雑なコマンドのAST表現

### 例1: 多段パイプライン
```bash
cat file.txt | grep "error" | sort | uniq -c | head -10
```
```
                    PIPE
                   /    \
                PIPE     head -10
               /    \
            PIPE     uniq -c
           /    \
        PIPE     sort
       /    \
    cat      grep "error"
    file.txt
```

### 例2: 複数リダイレクト
```bash
cmd < input1 < input2 > output 2>&1
```
```
NODE_COMMAND
├── args: ["cmd"]
└── redirects:
    ├── {op: "<", file: "input1"}
    ├── {op: "<", file: "input2"}
    ├── {op: ">", file: "output"}
    └── {op: "2>&1", file: NULL}  // 将来の拡張
```

## ASTの利点と課題

### 利点
1. **明確な構造**: コマンドの論理構造が一目瞭然
2. **拡張性**: 新しい構文要素の追加が容易
3. **最適化の可能性**: 実行前に構造を分析・最適化可能
4. **エラー検出**: 構文エラーを構築時に検出

### 現在の制限
1. **演算子の優先順位**: 現在はパイプのみサポート
2. **グルーピング**: サブシェル`()`未対応
3. **制御構造**: if/while/for未対応

## AST走査パターン

### 1. 深さ優先探索（実行時）
```c
int execute_ast(t_ast_node *node, t_ctx *ctx) {
    if (node->type == NODE_COMMAND) {
        return execute_command(node, ctx);
    } else if (node->type == NODE_PIPE) {
        // パイプ設定
        execute_ast(node->as.pipe.left, ctx);
        execute_ast(node->as.pipe.right, ctx);
    }
}
```

### 2. 幅優先探索（分析時）
- パイプラインの長さ計算
- 必要なプロセス数の事前計算

## 将来の拡張に向けたAST設計

### 1. 新しいノードタイプの追加
```c
typedef enum e_node_type {
    NODE_COMMAND,
    NODE_PIPE,
    NODE_AND,        // &&
    NODE_OR,         // ||
    NODE_SUBSHELL,   // ()
    NODE_REDIRECT,   // 独立したリダイレクトノード
} t_node_type;
```

### 2. より汎用的なノード構造
```c
struct s_ast_node {
    t_node_type type;
    void *data;              // ノード固有のデータ
    t_ast_node **children;   // 子ノードの配列
    size_t child_count;      // 子ノードの数
};
```

## ASTのデバッグとビジュアライゼーション

### AST出力関数の例
```c
void print_ast(t_ast_node *node, int depth) {
    for (int i = 0; i < depth; i++)
        printf("  ");
    
    if (node->type == NODE_COMMAND) {
        printf("CMD: ");
        t_word_list *arg = node->as.command.args;
        while (arg) {
            printf("%s ", arg->word->word);
            arg = arg->next;
        }
        printf("\n");
    } else if (node->type == NODE_PIPE) {
        printf("PIPE\n");
        print_ast(node->as.pipe.left, depth + 1);
        print_ast(node->as.pipe.right, depth + 1);
    }
}
```

## まとめ

ASTは単なるデータ構造ではなく、コマンドの意味を表現する重要な中間表現です。適切に設計されたASTにより：

1. **複雑なコマンドも単純な構造の組み合わせ**として表現
2. **実行エンジンとパーサーの分離**が可能
3. **将来の機能拡張**が容易

minishellのAST実装は、シンプルながら拡張可能な設計となっており、基本的なシェル機能を実現するのに十分な表現力を持っています。