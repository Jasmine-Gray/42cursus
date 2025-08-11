# 既知の問題

## 優先度の説明

- 🔴 **CRITICAL**: シェルがクラッシュする、データ損失の可能性
- 🟡 **HIGH**: 基本機能が動作しない、bashとの重大な非互換
- 🟢 **MEDIUM**: 特定の状況でのみ発生、回避策あり
- 🔵 **LOW**: 見た目の問題、エラーメッセージの相違

## 🔴 CRITICAL - シェルクラッシュ

### Issue #31: Heredoc中のCtrl+Cでminishell終了
**現象**: 
```bash
minishell$ cat << EOF
> ^C  # ここでCtrl+C
# minishellが終了してしまう
```
**期待動作**: heredocのみキャンセルし、プロンプトに戻る
**影響**: ユーザーの作業が失われる可能性

### Issue #32: コマンド実行中のシグナルでminishell終了
**現象**:
```bash
minishell$ cat
^C  # または ^\ 
# minishellが終了してしまう
```
**期待動作**: コマンドのみ終了し、プロンプトに戻る
**影響**: 予期しないシェル終了

## 🟡 HIGH - 動作不良

### Issue #26: 空のクォートがcommand not foundにならない
**現象**:
```bash
minishell$ ""
# エラーが出ない or 異なるエラー
```
**期待動作**:
```bash
bash$ ""
bash: : command not found
```

### Issue #33: 無効なコマンドチェーンでゴミ出力
**現象**:
```bash
minishell$ a | b | c
# ゴミ文字や破損した出力
```
**期待動作**: きれいなcommand not foundメッセージ

### Issue #35: パイプ構文エラーの終了コード
**現象**:
```bash
minishell$ | > |
minishell$ echo $?
0  # 誤り
```
**期待動作**: 非ゼロの終了コード

## 🟢 MEDIUM - 検証エラー

### Issue #27: unsetの引数検証
**現象**:
```bash
minishell$ unset ""
# エラーが出ない
```
**期待動作**: `not a valid identifier`エラー

### Issue #30: exportの識別子検証
**現象**:
```bash
minishell$ export 123=value
# 受け入れられてしまう
```
**期待動作**: 数字で始まる識別子は拒否

### Issue #34: exitの数値オーバーフロー
**現象**:
```bash
minishell$ exit 99999999999999999999999
# エラーなし
```
**期待動作**: `numeric argument required`エラー

## 🟢 MEDIUM - 変数・リダイレクト

### Issue #28: Heredocのクォート区切り文字
**現象**:
```bash
minishell$ cat << "EOF"
> $USER
> EOF
username  # 展開されてしまう
```
**期待動作**: `$USER`をそのまま出力（展開しない）

### Issue #29: ドルクォート構文
**現象**:
```bash
minishell$ echo $"test"
test  # 誤り
```
**期待動作**: 空出力（サポートしない構文）

### Issue #36: 未定義変数へのリダイレクト
**現象**:
```bash
minishell$ > $UNDEFINED
# 異なる終了コード
```
**期待動作**: `ambiguous redirect`エラー

### Issue #37: 複数ドル記号のパース
**現象**:
```bash
minishell$ echo $$$$$$$USER
# 異なる出力
```
**期待動作**: bashと同じパース規則

### Issue #38: スペースを含む変数へのリダイレクト
**現象**:
```bash
minishell$ export A="a b"
minishell$ > $A
# 不正な動作
```
**期待動作**: `ambiguous redirect`エラー

### Issue #39: 変数をコマンドとして実行
**現象**:
```bash
minishell$ export A="echo hello"
minishell$ $A
# command not foundが出ない
```
**期待動作**: `echo hello`という名前のコマンドを探す

### Issue #40: 空変数への曖昧なリダイレクト
**現象**:
```bash
minishell$ echo > $EMPTY_VAR
# エラーメッセージが異なる
```
**期待動作**: `ambiguous redirect`エラー

## 🔵 LOW - メッセージ・フォーマット

### Issue #42: envコマンドのエラーメッセージ
**現象**:
```bash
minishell$ env nonexistent
# エラーメッセージのフォーマットが異なる
```
**期待動作**: bashと同じエラーフォーマット

### Issue #41: パイプ内のビルトイン
**現象**:
```bash
minishell$ builtin | cat
builtin: command not found
```
**注**: 'builtin'自体はコマンドではない（正しい動作）

## 未実装機能（仕様）

これらは42プロジェクトの範囲外：

### 論理演算子
- `&&` - AND演算子
- `||` - OR演算子

### 制御構造
- `;` - コマンド区切り
- `&` - バックグラウンド実行

### 展開機能
- `*`, `?` - グロブ展開
- `$(...)` - コマンド置換
- `` `...` `` - バックティック
- `$((...)` - 算術展開
- `{...}` - ブレース展開

### その他
- `()` - サブシェル
- `[]` - 条件式

## 修正の進捗

| カテゴリ | 総数 | 修正済 | 進捗 |
|---------|------|--------|------|
| CRITICAL | 2 | 0 | 0% |
| HIGH | 3 | 0 | 0% |
| MEDIUM | 13 | 0 | 0% |
| LOW | 2 | 0 | 0% |
| **合計** | **20** | **0** | **0%** |

## 回避策

### シグナル問題の回避
- 現在: Ctrl+Cを避ける
- 推奨: 別ターミナルからkillコマンド使用

### 変数展開の回避
- 現在: 複雑な変数展開を避ける
- 推奨: シンプルな変数のみ使用

### リダイレクトの回避
- 現在: 変数を使ったリダイレクトを避ける
- 推奨: 直接ファイル名を指定

## テスト方法

### 自動テスト
```bash
# 既知の問題をテスト
bash tests/test_todo.sh

# 修正確認
bash tests/test_working.sh
```

### 手動テスト
シグナルハンドリングなど、対話的なテストが必要な項目は手動で確認

## 貢献方法

1. Issueを選択
2. ブランチ作成: `fix/issue-番号`
3. 修正実装
4. テスト追加/更新
5. PR作成

## 更新履歴

- 2024-08-08: 初版作成、20個の既知の問題を文書化
- (今後の修正時に更新)