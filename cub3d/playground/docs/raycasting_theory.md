# レイキャスティング理論 完全ガイド

このドキュメントは cub3D 実装に必要なレイキャスティングの理論を詳細に解説します。

## 目次
1. [全体の流れ](#1-全体の流れ)
2. [ベクトル表現](#2-ベクトル表現)
3. [DDAアルゴリズム](#3-ddaアルゴリズム)
4. [魚眼補正](#4-魚眼補正)
5. [実装のポイント](#5-実装のポイント)

---

## 1. 全体の流れ

### レイキャスティングとは

2Dマップから擬似3D視点を生成する技術。Wolfenstein 3D (1992) で有名になった。

**処理の流れ：**
```
┌─────────────────────────────────────────────────────────────┐
│                     毎フレームの処理                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 画面をクリア（または天井・床を描画）                        │
│                    ↓                                        │
│  2. 画面の各 x 座標（0 ~ WIDTH-1）について：                   │
│     ┌─────────────────────────────────────────────┐        │
│     │ a. レイの方向を計算                          │        │
│     │ b. DDAで壁までの距離を計算                    │        │
│     │ c. 壁の高さを計算                            │        │
│     │ d. 縦線を描画                               │        │
│     └─────────────────────────────────────────────┘        │
│                    ↓                                        │
│  3. 画面を更新（mlx_put_image_to_window）                    │
│                    ↓                                        │
│  4. 入力処理（移動・回転）                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 擬似コード全体像

```c
void render_frame(t_game *game)
{
    // 天井と床を描画
    draw_ceiling(game);
    draw_floor(game);

    // 画面の各縦線について
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        // レイの方向を計算
        t_ray ray = calculate_ray(game->player, x);

        // DDAで壁を探す
        t_hit hit = cast_ray(game->map, ray);

        // 壁の高さを計算
        int wall_height = calculate_wall_height(hit.distance);

        // 縦線を描画
        draw_vertical_line(game->img, x, wall_height, hit.side);
    }

    // 画面に表示
    mlx_put_image_to_window(game->mlx, game->win, game->img.ptr, 0, 0);
}
```

---

## 2. ベクトル表現

### なぜベクトルを使うのか

従来の方法（角度ベース）：
```c
// 悪い例：角度ベース
double player_angle = 0.0;  // ラジアン
double fov = 60.0 * (M_PI / 180.0);

// 毎回 sin/cos を計算 → 遅い！
for (int x = 0; x < WIDTH; x++)
{
    double ray_angle = player_angle - fov/2 + fov * x / WIDTH;
    double ray_dir_x = cos(ray_angle);  // 毎回計算
    double ray_dir_y = sin(ray_angle);  // 毎回計算
}
```

ベクトルベース（推奨）：
```c
// 良い例：ベクトルベース
double dir_x = 1.0, dir_y = 0.0;        // 方向ベクトル
double plane_x = 0.0, plane_y = 0.66;   // カメラ平面

// sin/cos は回転時のみ
for (int x = 0; x < WIDTH; x++)
{
    double camera_x = 2 * x / (double)WIDTH - 1;  // -1 ~ +1
    double ray_dir_x = dir_x + plane_x * camera_x;  // 加算だけ！
    double ray_dir_y = dir_y + plane_y * camera_x;  // 高速！
}
```

### 3つのベクトルの関係

```
                        カメラ平面 (plane)
                    ←───────────────────→
                          │
                          │ 方向ベクトル (dir)
                          │
                          │
                          ▼

                    ◉ プレイヤー位置 (pos)


[上から見た図]

              plane_x, plane_y
           ←─────────────────────→
           │                     │
           │    視野 (FOV)       │
           │   ╱         ╲      │
           │  ╱           ╲     │
           │ ╱             ╲    │
           │╱               ╲   │
           ◉ ─────────────────→ dir_x, dir_y
        pos_x, pos_y
```

### 視野角 (FOV) の計算

FOV は `dir` と `plane` の長さの比率で決まる：

```
FOV = 2 * atan(|plane| / |dir|)
```

**具体例：**
```c
// 66度の視野角（Wolfenstein 3Dと同じ）
double dir_x = 1.0, dir_y = 0.0;        // |dir| = 1.0
double plane_x = 0.0, plane_y = 0.66;   // |plane| = 0.66

// FOV = 2 * atan(0.66 / 1.0) ≈ 66度
```

| dir の長さ | plane の長さ | FOV |
|-----------|-------------|-----|
| 1.0 | 0.66 | 約66度 |
| 1.0 | 1.0 | 90度 |
| 1.0 | 0.5 | 約53度 |
| 1.0 | 2.0 | 約127度 |

### レイ方向の計算詳細

```c
// x = 画面上の横座標（0 ~ SCREEN_WIDTH - 1）

// Step 1: カメラ空間での x 座標（-1 ~ +1）
double camera_x = 2.0 * x / (double)SCREEN_WIDTH - 1.0;
//              └─ 0~WIDTH を 0~2 に変換 ─┘  └─ -1 にシフト ─┘

// 例：WIDTH = 640 の場合
// x = 0   → camera_x = -1.0  （画面左端）
// x = 320 → camera_x = 0.0   （画面中央）
// x = 639 → camera_x = 0.997 （画面右端）

// Step 2: レイの方向ベクトル
double ray_dir_x = dir_x + plane_x * camera_x;
double ray_dir_y = dir_y + plane_y * camera_x;
```

**図解：**
```
        camera_x = -1        camera_x = 0       camera_x = +1
              ↓                   ↓                  ↓
              ┌───────────────────┬───────────────────┐
              │                   │                   │  画面
              └───────────────────┴───────────────────┘
                  ↖               ↑               ↗
                    ↖             │             ↗
                      ↖           │           ↗
               ray(-1)  ↖         │ray(0)   ↗  ray(+1)
                          ↖       │       ↗
                            ↖     │     ↗
                              ◉───┴───→ dir
                              │
                           plane
```

### 回転の実装

プレイヤーが左右に回転するとき、`dir` と `plane` の両方を回転：

```c
void rotate_player(t_player *p, double angle)
{
    // 回転行列を使用
    // [cos(θ)  -sin(θ)] [x]   [x*cos(θ) - y*sin(θ)]
    // [sin(θ)   cos(θ)] [y] = [x*sin(θ) + y*cos(θ)]

    double cos_a = cos(angle);
    double sin_a = sin(angle);

    // 方向ベクトルを回転
    double old_dir_x = p->dir_x;
    p->dir_x = p->dir_x * cos_a - p->dir_y * sin_a;
    p->dir_y = old_dir_x * sin_a + p->dir_y * cos_a;

    // カメラ平面も同様に回転
    double old_plane_x = p->plane_x;
    p->plane_x = p->plane_x * cos_a - p->plane_y * sin_a;
    p->plane_y = old_plane_x * sin_a + p->plane_y * cos_a;
}
```

---

## 3. DDAアルゴリズム

### DDA (Digital Differential Analysis) とは

グリッド上で線を描画する際に、どのセルを通過するかを効率的に計算するアルゴリズム。

### なぜ単純なステップでは駄目なのか

```
❌ 悪い方法：固定ステップ
┌───┬───┬───┬───┬───┐
│   │   │   │ █ │ █ │
├───┼───┼───┼───┼───┤
│   │   │ · │ █ │   │  · = チェックポイント
├───┼───┼───┼───┼───┤
│   │ · │   │   │   │  壁を飛び越える可能性！
├───┼───┼───┼───┼───┤
│ · │   │   │   │   │
├───┼───┼───┼───┼───┤
│ ◉ │   │   │   │   │
└───┴───┴───┴───┴───┘

✅ 良い方法：DDA（グリッド境界でチェック）
┌───┬───┬───┬───┬───┐
│   │   │   │ █ │ █ │
├───┼───┼───┼*──┼───┤  * = グリッド境界でのチェック
│   │   │ * │ █ │   │
├───┼───┼*──┼───┼───┤  確実に壁を検出！
│   │ * │   │   │   │
├───┼*──┼───┼───┼───┤
│ ◉ │   │   │   │   │
└───┴───┴───┴───┴───┘
```

### 核心となる変数

```c
// 現在のマップ座標（整数）
int map_x = (int)pos_x;
int map_y = (int)pos_y;

// deltaDistX: レイがX方向に1マス進むときの距離
// deltaDistY: レイがY方向に1マス進むときの距離
double delta_dist_x = fabs(1.0 / ray_dir_x);
double delta_dist_y = fabs(1.0 / ray_dir_y);

// sideDistX: 次のX境界までの距離
// sideDistY: 次のY境界までの距離
double side_dist_x, side_dist_y;

// stepX, stepY: 進む方向（+1 または -1）
int step_x, step_y;
```

### deltaDistの幾何学的意味

```
deltaDistX = レイが X 方向に 1 単位進むときの、レイの長さ

        │
        │ ← 1単位
        │
    ────┼────────────────
        │      ╱
        │    ╱  deltaDistX
        │  ╱
        │╱ θ
    ────◉────────────────
        │

deltaDistX = 1 / cos(θ) = 1 / ray_dir_x（ray_dir が正規化されている場合）
           = √(1 + (ray_dir_y/ray_dir_x)²)

簡易式（正規化不要）：
deltaDistX = |1 / ray_dir_x|
deltaDistY = |1 / ray_dir_y|
```

### sideDistの初期化

```c
// レイの方向によって初期化が変わる
if (ray_dir_x < 0)  // 左に向かっている
{
    step_x = -1;
    // 現在位置からマップ左端までの距離
    side_dist_x = (pos_x - map_x) * delta_dist_x;
}
else  // 右に向かっている
{
    step_x = 1;
    // 現在位置からマップ右端までの距離
    side_dist_x = (map_x + 1.0 - pos_x) * delta_dist_x;
}

// Y方向も同様
if (ray_dir_y < 0)  // 上に向かっている
{
    step_y = -1;
    side_dist_y = (pos_y - map_y) * delta_dist_y;
}
else  // 下に向かっている
{
    step_y = 1;
    side_dist_y = (map_y + 1.0 - pos_y) * delta_dist_y;
}
```

**図解（step_x = 1, step_y = -1 の場合）：**
```
        map_x    map_x+1
           │        │
    ───────┼────────┼───────  map_y
           │   ↗    │
           │ ╱      │
           │╱ ◉     │         ◉ = pos_x, pos_y
           │        │
    ───────┼────────┼───────  map_y+1
           │        │

    side_dist_x = (map_x + 1 - pos_x) * delta_dist_x
    side_dist_y = (pos_y - map_y) * delta_dist_y
```

### DDAループの詳細

```c
int hit = 0;   // 壁に当たったか
int side;      // 0 = X面（縦の壁）, 1 = Y面（横の壁）

while (hit == 0)
{
    // より近い境界に進む
    if (side_dist_x < side_dist_y)
    {
        side_dist_x += delta_dist_x;  // 次のX境界まで更新
        map_x += step_x;              // X方向に1マス進む
        side = 0;                     // 縦の壁に当たる予定
    }
    else
    {
        side_dist_y += delta_dist_y;  // 次のY境界まで更新
        map_y += step_y;              // Y方向に1マス進む
        side = 1;                     // 横の壁に当たる予定
    }

    // 壁チェック
    if (map[map_x][map_y] > 0)
        hit = 1;
}
```

**ループの進行例：**
```
初期状態:
┌───┬───┬───┬───┐
│   │   │ █ │ █ │  map_x=0, map_y=2
├───┼───┼───┼───┤  side_dist_x=0.5, side_dist_y=0.8
│   │   │ █ │   │
├───┼───┼───┼───┤
│ ◉→│   │   │   │
└───┴───┴───┴───┘

Step 1: side_dist_x(0.5) < side_dist_y(0.8) → X方向に進む
┌───┬───┬───┬───┐
│   │   │ █ │ █ │  map_x=1, map_y=2
├───┼───┼───┼───┤  side_dist_x=1.5, side_dist_y=0.8
│   │   │ █ │   │
├───┼───┼───┼───┤
│   │ * │   │   │  * = 現在のチェック位置
└───┴───┴───┴───┘

Step 2: side_dist_x(1.5) > side_dist_y(0.8) → Y方向に進む
┌───┬───┬───┬───┐
│   │   │ █ │ █ │  map_x=1, map_y=1
├───┼───┼───┼───┤  side_dist_x=1.5, side_dist_y=1.6
│   │ * │ █ │   │  * = 現在のチェック位置
├───┼───┼───┼───┤
│   │   │   │   │
└───┴───┴───┴───┘

Step 3: side_dist_x(1.5) < side_dist_y(1.6) → X方向に進む
┌───┬───┬───┬───┐
│   │   │ █ │ █ │  map_x=2, map_y=1
├───┼───┼───┼───┤  map[2][1] = 1 → 壁発見！ hit = 1
│   │   │ █ │   │
├───┼───┼───┼───┤
│   │   │   │   │
└───┴───┴───┴───┘
```

---

## 4. 魚眼補正

### 魚眼効果とは

```
❌ ユークリッド距離（実際の距離）を使った場合：

    画面中央の壁は正常だが、
    画面端の壁が膨らんで見える

    ┌─────────────────────────────┐
    │  ╲                     ╱   │
    │   ╲                   ╱    │  ← 端が膨らむ
    │    ╲                 ╱     │
    │     │               │      │
    │     │               │      │
    │    ╱                 ╲     │
    │   ╱                   ╲    │
    │  ╱                     ╲   │
    └─────────────────────────────┘
```

**原因：**
画面端に向かうレイは斜めに飛ぶため、同じ壁でも端のレイは長い距離を計測する。

### なぜ起こるのか（数学的説明）

```
              壁
               │
               │
    ──────────→│  中央のレイ：距離 = d
               │
              ╱│
            ╱  │
          ╱    │  端のレイ：距離 = d / cos(θ) > d
        ╱      │
      ╱ θ     │
    ◉──────────┘
  プレイヤー

端のレイは長い → 壁が小さく描画される → 端が凹む
→ 逆に言えば、中央が膨らんで見える（魚眼効果）
```

### 解決策：垂直距離を使う

```
perpWallDist = カメラ平面への垂直距離

              壁
               │
               │
    ══════════→│  perpWallDist（垂直距離）
    ══════════→│  = すべてのレイで同じ！
               │
              ╱│
            ╱  │
          ╱    │
        ╱      │
      ╱        │
    ◉──────────┘

すべてのレイで同じ垂直距離 → 壁が均一に描画される
```

### perpWallDist の計算

**DDA終了後の計算：**

```c
double perp_wall_dist;

if (side == 0)  // 縦の壁（X面）に当たった
{
    // side_dist_x は「次の」X境界までの距離なので、
    // 1つ前の境界（実際に当たった壁）までの距離を計算
    perp_wall_dist = side_dist_x - delta_dist_x;
}
else  // 横の壁（Y面）に当たった
{
    perp_wall_dist = side_dist_y - delta_dist_y;
}
```

**なぜこの式で垂直距離が求まるのか：**

```
delta_dist_x は「レイ方向に1マス進むときのレイの長さ」

DDAループで:
  side_dist_x += delta_dist_x  を繰り返すと、
  side_dist_x = 「プレイヤーから現在位置までのレイの長さ」

しかし side_dist_x は「次の」境界までの距離を持っているので、
perp_wall_dist = side_dist_x - delta_dist_x
              = 「壁に当たった境界までの距離」

この値は自動的にカメラ平面への垂直距離になる！
（ベクトルの内積の性質による）
```

### 壁の高さ計算

```c
int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);

// 描画開始・終了位置
int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;

// 画面外にはみ出る場合の処理
if (draw_start < 0)
    draw_start = 0;
if (draw_end >= SCREEN_HEIGHT)
    draw_end = SCREEN_HEIGHT - 1;
```

**図解：**
```
        perp_wall_dist = 1.0      perp_wall_dist = 2.0

        ┌─────┐                   ┌─────┐
        │     │                   │     │
        │     │                   │ ─── │  ← 半分の高さ
        │ ███ │  ← 高さ = 480     │ ███ │  ← 高さ = 240
        │ ███ │                   │ ─── │
        │     │                   │     │
        └─────┘                   └─────┘

        近い壁 = 大きい           遠い壁 = 小さい
```

---

## 5. 実装のポイント

### 最適化テクニック

**1. ルックアップテーブル（回転用）**
```c
// 起動時に計算
#define ROT_SPEED 0.05
double g_cos_rot = cos(ROT_SPEED);
double g_sin_rot = sin(ROT_SPEED);
double g_cos_rot_neg = cos(-ROT_SPEED);
double g_sin_rot_neg = sin(-ROT_SPEED);
```

**2. 除算の回避**
```c
// 悪い例
double delta_dist_x = sqrt(1 + (ray_dir_y * ray_dir_y) / (ray_dir_x * ray_dir_x));

// 良い例
double delta_dist_x = fabs(1.0 / ray_dir_x);
// ray_dir_x が 0 の場合は無限大になるが、
// fabs() は正しく INFINITY を返す
```

**3. 整数演算の活用**
```c
// マップ座標は整数で管理
int map_x = (int)pos_x;
int map_y = (int)pos_y;
```

### よくあるバグと対策

**1. ゼロ除算**
```c
// ray_dir_x または ray_dir_y が 0 の場合
// fabs(1.0 / 0.0) は INFINITY を返す
// → side_dist 比較で正しく処理される
```

**2. マップ境界外アクセス**
```c
// DDAループ内でマップ範囲チェック
if (map_x < 0 || map_x >= MAP_WIDTH ||
    map_y < 0 || map_y >= MAP_HEIGHT)
{
    hit = 1;  // 境界外は壁として扱う
    break;
}
```

**3. 壁の色分け（デバッグ用）**
```c
// side の値で色を変えると、N/S/E/W の区別が視覚的にわかる
int color;
if (side == 0)
    color = (step_x > 0) ? COLOR_EAST : COLOR_WEST;
else
    color = (step_y > 0) ? COLOR_SOUTH : COLOR_NORTH;
```

### デバッグのヒント

1. **最初はテクスチャなしで実装**
   - 単色の壁で動作確認
   - side で色を変えて東西南北を確認

2. **小さいマップで開始**
   ```c
   int map[8][8] = {
       {1,1,1,1,1,1,1,1},
       {1,0,0,0,0,0,0,1},
       {1,0,0,0,0,0,0,1},
       {1,0,0,0,0,0,0,1},
       {1,0,0,0,0,0,0,1},
       {1,0,0,0,0,0,0,1},
       {1,0,0,0,0,0,0,1},
       {1,1,1,1,1,1,1,1},
   };
   ```

3. **2Dマップも同時に描画**
   - プレイヤー位置とレイの方向を2Dで表示
   - デバッグが格段に楽になる

---

## 参考資料

- [Lode's Computer Graphics Tutorial - Raycasting](https://lodev.org/cgtutor/raycasting.html)
- [Permadi's Ray-Casting Tutorial](https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/)
- Wolfenstein 3D ソースコード

---

## 次のステップ

1. [x] 理論を理解する（このドキュメント）
2. [ ] 簡易プロトタイプを実装する
3. [ ] テクスチャを追加する
4. [ ] 床・天井を描画する
5. [ ] 最適化する
