# minilibX 基礎ガイド

このドキュメントは minilibX の使い方と描画原理を詳細に解説します。

## 目次
1. [描画の原理](#1-描画の原理)
2. [基本的なAPI](#2-基本的なapi)
3. [イメージ描画](#3-イメージ描画)
4. [イベント処理](#4-イベント処理)
5. [実装例](#5-実装例)

---

## 1. 描画の原理

### コンピュータグラフィックスの基本

**フレームバッファ方式：**
```
┌─────────────────────────────────────────────────────────────┐
│                        RAM（メモリ）                         │
│                                                             │
│   ┌─────────────────────────────────────────────────────┐  │
│   │              イメージバッファ                         │  │
│   │                                                     │  │
│   │   ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┐      │  │
│   │   │BGRA │BGRA │BGRA │BGRA │BGRA │BGRA │... │      │  │
│   │   └─────┴─────┴─────┴─────┴─────┴─────┴─────┘      │  │
│   │   ↑                                                 │  │
│   │   各ピクセル = 4バイト（Blue, Green, Red, Alpha）     │  │
│   │                                                     │  │
│   └─────────────────────────────────────────────────────┘  │
│                           │                                 │
│                           │ mlx_put_image_to_window()       │
│                           ▼                                 │
│   ┌─────────────────────────────────────────────────────┐  │
│   │              ウィンドウ（画面）                       │  │
│   └─────────────────────────────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### なぜイメージバッファを使うのか

**ダブルバッファリングの利点：**

```
❌ 直接描画（ちらつきが発生）

    Frame 1: 描画中...
    ┌───────────────┐
    │ ████          │  ← ユーザーには描画途中が見える
    │               │
    └───────────────┘

    Frame 2: まだ描画中...
    ┌───────────────┐
    │ ██████████    │  ← ちらつきの原因
    │               │
    └───────────────┘

✅ イメージバッファ経由（スムーズ）

    1. バッファに描画（見えない）
    2. 完成したら一気に転送
    ┌───────────────┐
    │ ██████████████│  ← ユーザーには完成した絵だけ見える
    │ ██████████████│
    └───────────────┘
```

### ピクセルのメモリレイアウト

```
img_addr（イメージバッファの先頭アドレス）
    │
    ▼
    ┌──────────────────────────────────────────────────────────┐
    │ B G R A │ B G R A │ B G R A │ ... │ padding │           │ ← Row 0
    ├──────────────────────────────────────────────────────────┤
    │ B G R A │ B G R A │ B G R A │ ... │ padding │           │ ← Row 1
    ├──────────────────────────────────────────────────────────┤
    │ B G R A │ B G R A │ B G R A │ ... │ padding │           │ ← Row 2
    ├──────────────────────────────────────────────────────────┤
    │  ...                                                     │
    └──────────────────────────────────────────────────────────┘

    ├────────────────────── size_line ────────────────────────┤
```

**重要な変数：**
- `img_addr` - バッファの先頭アドレス
- `bpp` (bits per pixel) - 1ピクセルあたりのビット数（通常32）
- `size_line` - 1行あたりのバイト数（パディング含む）
- `endian` - バイトオーダー（0=little endian, 1=big endian）

---

## 2. 基本的なAPI

### 初期化と終了

```c
#include "mlx.h"

// MLX接続の初期化
void *mlx = mlx_init();
if (!mlx)
    return (error_exit("MLX init failed"));

// ウィンドウ作成
void *win = mlx_new_window(mlx, WIDTH, HEIGHT, "Window Title");
if (!win)
    return (error_exit("Window creation failed"));

// メインループ開始（ブロッキング）
mlx_loop(mlx);

// 終了時（mlx_loopの後は実行されないので、フック内で呼ぶ）
mlx_destroy_window(mlx, win);
mlx_destroy_display(mlx);  // Linux のみ
free(mlx);                  // Linux のみ
```

### イメージの作成と操作

```c
// イメージ作成
void *img_ptr = mlx_new_image(mlx, WIDTH, HEIGHT);
if (!img_ptr)
    return (error_exit("Image creation failed"));

// ピクセルデータへのアクセス情報を取得
int bpp;        // bits per pixel（通常32）
int size_line;  // 1行のバイト数
int endian;     // バイトオーダー

char *img_addr = mlx_get_data_addr(img_ptr, &bpp, &size_line, &endian);

// イメージを画面に表示
mlx_put_image_to_window(mlx, win, img_ptr, 0, 0);
//                                          ↑  ↑
//                                          x  y 座標（左上が原点）

// イメージの破棄
mlx_destroy_image(mlx, img_ptr);
```

---

## 3. イメージ描画

### ピクセルの書き込み

```c
void put_pixel(char *img_addr, int x, int y, int color,
               int size_line, int bpp)
{
    char *dst;

    // ピクセル位置の計算
    //   y * size_line  → y行目の先頭アドレス
    //   x * (bpp / 8)  → x列目のオフセット（bpp=32なら4バイト）
    dst = img_addr + (y * size_line + x * (bpp / 8));

    // 色の書き込み（4バイト = BGRA）
    *(unsigned int *)dst = color;
}
```

### 色の表現

```c
// 色のフォーマット: 0x00RRGGBB または 0xAARRGGBB

// 基本色
#define BLACK   0x00000000
#define WHITE   0x00FFFFFF
#define RED     0x00FF0000
#define GREEN   0x0000FF00
#define BLUE    0x000000FF

// RGB値から色を作成
int create_color(int r, int g, int b)
{
    return ((r << 16) | (g << 8) | b);
}

// 色の分解
int get_r(int color) { return ((color >> 16) & 0xFF); }
int get_g(int color) { return ((color >> 8) & 0xFF); }
int get_b(int color) { return (color & 0xFF); }
```

### 縦線の描画（レイキャスティングの基本）

```c
void draw_vertical_line(t_img *img, int x, int y_start, int y_end, int color)
{
    int y;

    // 範囲チェック
    if (y_start < 0)
        y_start = 0;
    if (y_end >= SCREEN_HEIGHT)
        y_end = SCREEN_HEIGHT - 1;

    y = y_start;
    while (y <= y_end)
    {
        put_pixel(img, x, y, color);
        y++;
    }
}
```

### 画面のクリア

```c
void clear_image(t_img *img)
{
    int x;
    int y;

    y = 0;
    while (y < SCREEN_HEIGHT)
    {
        x = 0;
        while (x < SCREEN_WIDTH)
        {
            put_pixel(img, x, y, 0x000000);  // 黒で塗りつぶし
            x++;
        }
        y++;
    }
}

// より高速な方法（memsetを使用）
void clear_image_fast(t_img *img)
{
    ft_memset(img->addr, 0, SCREEN_HEIGHT * img->size_line);
}
```

---

## 4. イベント処理

### キーボード入力

```c
// キーコード定義（Linux/macOS で異なる）
#ifdef __linux__
# define KEY_ESC    65307
# define KEY_W      119
# define KEY_A      97
# define KEY_S      115
# define KEY_D      100
# define KEY_LEFT   65361
# define KEY_RIGHT  65363
#else  // macOS
# define KEY_ESC    53
# define KEY_W      13
# define KEY_A      0
# define KEY_S      1
# define KEY_D      2
# define KEY_LEFT   123
# define KEY_RIGHT  124
#endif

// キーイベントハンドラ
int key_press(int keycode, t_game *game)
{
    if (keycode == KEY_ESC)
        close_game(game);
    else if (keycode == KEY_W)
        game->keys.w = 1;
    else if (keycode == KEY_S)
        game->keys.s = 1;
    // ... 他のキー
    return (0);
}

int key_release(int keycode, t_game *game)
{
    if (keycode == KEY_W)
        game->keys.w = 0;
    else if (keycode == KEY_S)
        game->keys.s = 0;
    // ... 他のキー
    return (0);
}

// フック登録
mlx_hook(win, 2, 1L<<0, key_press, &game);    // KeyPress
mlx_hook(win, 3, 1L<<1, key_release, &game);  // KeyRelease
```

### ウィンドウ閉じるボタン

```c
int close_window(t_game *game)
{
    // クリーンアップ処理
    mlx_destroy_image(game->mlx, game->img.ptr);
    mlx_destroy_window(game->mlx, game->win);
    mlx_destroy_display(game->mlx);  // Linux のみ
    free(game->mlx);                  // Linux のみ
    exit(0);
    return (0);
}

// フック登録（X11イベント17 = DestroyNotify）
mlx_hook(win, 17, 0, close_window, &game);
```

### ゲームループ

```c
// 方法1: mlx_loop_hook（毎フレーム呼ばれる）
int game_loop(t_game *game)
{
    // 入力処理
    process_input(game);

    // 更新
    update_player(game);

    // 描画
    render_frame(game);

    return (0);
}

mlx_loop_hook(mlx, game_loop, &game);
mlx_loop(mlx);  // ブロッキング

// 方法2: mlx_hook でフレーム管理
mlx_hook(win, 2, 1L<<0, key_press, &game);      // キー押下
mlx_hook(win, 3, 1L<<1, key_release, &game);    // キー離す
mlx_loop_hook(mlx, game_loop, &game);           // 毎フレーム
mlx_loop(mlx);
```

### X11イベントコード一覧

| イベント | コード | マスク | 説明 |
|---------|--------|--------|------|
| KeyPress | 2 | 1L<<0 | キー押下 |
| KeyRelease | 3 | 1L<<1 | キー離す |
| ButtonPress | 4 | 1L<<2 | マウスボタン押下 |
| ButtonRelease | 5 | 1L<<3 | マウスボタン離す |
| MotionNotify | 6 | 1L<<6 | マウス移動 |
| DestroyNotify | 17 | 0 | ウィンドウ閉じる |

---

## 5. 実装例

### 完全な初期化コード

```c
typedef struct s_img
{
    void    *ptr;       // イメージポインタ
    char    *addr;      // ピクセルデータアドレス
    int     bpp;        // bits per pixel
    int     size_line;  // 1行のバイト数
    int     endian;     // バイトオーダー
}   t_img;

typedef struct s_game
{
    void    *mlx;
    void    *win;
    t_img   img;
    // ... 他のゲームデータ
}   t_game;

int init_mlx(t_game *game)
{
    // MLX初期化
    game->mlx = mlx_init();
    if (!game->mlx)
        return (1);

    // ウィンドウ作成
    game->win = mlx_new_window(game->mlx, WIDTH, HEIGHT, "cub3D");
    if (!game->win)
    {
        free(game->mlx);
        return (1);
    }

    // イメージ作成
    game->img.ptr = mlx_new_image(game->mlx, WIDTH, HEIGHT);
    if (!game->img.ptr)
    {
        mlx_destroy_window(game->mlx, game->win);
        free(game->mlx);
        return (1);
    }

    // イメージデータアドレス取得
    game->img.addr = mlx_get_data_addr(
        game->img.ptr,
        &game->img.bpp,
        &game->img.size_line,
        &game->img.endian
    );

    return (0);
}
```

### テスト用描画（グラデーション）

```c
void draw_gradient(t_game *game)
{
    int x;
    int y;
    int color;

    y = 0;
    while (y < HEIGHT)
    {
        x = 0;
        while (x < WIDTH)
        {
            // 位置に基づいた色を生成
            color = ((x * 255 / WIDTH) << 16) |    // R
                    ((y * 255 / HEIGHT) << 8) |     // G
                    ((x + y) * 127 / (WIDTH + HEIGHT)); // B
            put_pixel(&game->img, x, y, color);
            x++;
        }
        y++;
    }
    mlx_put_image_to_window(game->mlx, game->win, game->img.ptr, 0, 0);
}
```

### main関数テンプレート

```c
int main(int argc, char **argv)
{
    t_game  game;

    // 引数チェック
    if (argc != 2)
        return (ft_putstr_fd("Usage: ./cub3D <map.cub>\n", 2), 1);

    // ゲーム構造体初期化
    ft_memset(&game, 0, sizeof(t_game));

    // マップ読み込み
    if (parse_map(argv[1], &game))
        return (1);

    // MLX初期化
    if (init_mlx(&game))
        return (cleanup(&game), 1);

    // イベントフック設定
    mlx_hook(game.win, 2, 1L<<0, key_press, &game);
    mlx_hook(game.win, 3, 1L<<1, key_release, &game);
    mlx_hook(game.win, 17, 0, close_window, &game);
    mlx_loop_hook(game.mlx, game_loop, &game);

    // メインループ開始
    mlx_loop(game.mlx);

    return (0);
}
```

---

## トラブルシューティング

### よくある問題

**1. セグメンテーションフォルト**
```c
// 原因: 画面外への書き込み
void put_pixel_safe(t_img *img, int x, int y, int color)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;  // 範囲外は無視
    // ... 通常の描画処理
}
```

**2. 画面が真っ暗**
```c
// 原因1: mlx_put_image_to_window を呼んでいない
// 原因2: 色が 0x00000000（黒）
// 原因3: 描画位置が画面外

// デバッグ用：全画面を赤で塗る
void debug_fill_red(t_game *game)
{
    int i = 0;
    while (i < WIDTH * HEIGHT)
    {
        ((int *)game->img.addr)[i] = 0xFF0000;
        i++;
    }
    mlx_put_image_to_window(game->mlx, game->win, game->img.ptr, 0, 0);
}
```

**3. キーが反応しない**
```c
// 原因: フック登録が間違っている
// 確認: printf でキーコードを出力
int key_press(int keycode, t_game *game)
{
    printf("Key pressed: %d\n", keycode);  // デバッグ
    // ...
}
```

---

## 次のステップ

1. [x] minilibX の基礎を理解する（このドキュメント）
2. [ ] ウィンドウを開いてグラデーションを描画する
3. [ ] キー入力で色を変える
4. [ ] レイキャスティングを実装する
