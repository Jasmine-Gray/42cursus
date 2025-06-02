## 📘 pwd.c + libft コンパイル手順

---

### ✅ コマンド概要

```bash
cc pwd.c ../libft/*.c ../libft/ft_printf/*.c -I../libft -I../libft/ft_printf
```

このコマンドは `pwd.c` を自作ライブラリ `libft` と `ft_printf` とともにコンパイルします。

---

### 🔍 各要素の解説

#### 1. `cc`

* C言語のコンパイラ（多くの環境で `gcc` または `clang` にリンク）。

#### 2. `pwd.c`

* カレントディレクトリを取得し表示するメインプログラム。

#### 3. `../libft/*.c`

* `libft` のすべての `.c` ファイルをコンパイル対象に含めます。

  * 例：`ft_strlen.c`, `ft_strdup.c` など

#### 4. `../libft/ft_printf/*.c`

* `ft_printf` に関する `.c` ファイル群も追加します。

  * 例：`ft_printf.c`, `ft_putstr.c` など

#### 5. `-I../libft`, `-I../libft/ft_printf`

* `#include "libft.h"` や `#include "ft_printf.h"` に対応するため、ヘッダーファイルの検索パスを指定します。

---

### 📂 ディレクトリ構成

```text
.
├── pwd.c                     ← メインファイル
├── libft/
│   ├── ft_strlen.c           ← 文字列関数などのユーティリティ
│   ├── libft.h
│   └── ft_printf/
│       ├── ft_printf.c       ← printf 実装
│       ├── ft_putstr.c
│       └── ft_printf.h
```

---

### 💡 補足：静的ライブラリ `.a` を使う方法

通常は `.a` アーカイブを使う方法もあります：

```bash
cc pwd.c -L../libft -lft -I../libft
```

* `-L`：ライブラリディレクトリを指定
* `-lft`：`libft.a` をリンク
* `-I`：ヘッダーファイルの検索パスを指定

ただし、開発中は `.a` を作らず `.c` ファイルを直接コンパイルするのが便利です。

---

### 📋 まとめ表

| 部分                       | 意味                             |
| ------------------------ | ------------------------------ |
| `pwd.c`                  | メイン関数のソースコード                   |
| `../libft/*.c`           | `libft` の関数群をコンパイルに含める         |
| `../libft/ft_printf/*.c` | `ft_printf` 系のソースもコンパイル        |
| `-I../libft`             | `libft.h` を探すためのインクルードパス指定     |
| `-I../libft/ft_printf`   | `ft_printf.h` を探すためのインクルードパス指定 |

---