#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include <array>

// 定数
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int MAP_WIDTH = 10;
constexpr int MAP_HEIGHT = 10;
constexpr float PI = 3.14159265f;
constexpr float FOV = 60.0f * PI / 180.0f;  // 視野角 60度

// マップデータ (1 = 壁, 0 = 空間)
int worldMap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

// プレイヤー
struct Player {
    float x = 5.0f;
    float y = 5.0f;
    float angle = 0.0f;
    float moveSpeed = 0.05f;
    float rotSpeed = 0.03f;
};

// DDAアルゴリズムでレイキャスティング
float castRay(float playerX, float playerY, float rayAngle) {
    // レイの方向
    float rayDirX = cos(rayAngle);
    float rayDirY = sin(rayAngle);

    // 現在のマップセル
    int mapX = static_cast<int>(playerX);
    int mapY = static_cast<int>(playerY);

    // 次のX/Y境界までの距離
    float deltaDistX = (rayDirX == 0) ? 1e30f : std::abs(1.0f / rayDirX);
    float deltaDistY = (rayDirY == 0) ? 1e30f : std::abs(1.0f / rayDirY);

    float sideDistX, sideDistY;
    int stepX, stepY;

    // ステップ方向と初期サイド距離を計算
    if (rayDirX < 0) {
        stepX = -1;
        sideDistX = (playerX - mapX) * deltaDistX;
    } else {
        stepX = 1;
        sideDistX = (mapX + 1.0f - playerX) * deltaDistX;
    }
    if (rayDirY < 0) {
        stepY = -1;
        sideDistY = (playerY - mapY) * deltaDistY;
    } else {
        stepY = 1;
        sideDistY = (mapY + 1.0f - playerY) * deltaDistY;
    }

    // DDAループ
    bool hit = false;
    int side = 0;  // 0: X面に衝突, 1: Y面に衝突

    while (!hit) {
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }

        // マップ範囲チェック
        if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
            return 1e30f;
        }

        // 壁に衝突
        if (worldMap[mapY][mapX] > 0) {
            hit = true;
        }
    }

    // 垂直距離を計算（魚眼効果を避けるため）
    float perpWallDist;
    if (side == 0) {
        perpWallDist = sideDistX - deltaDistX;
    } else {
        perpWallDist = sideDistY - deltaDistY;
    }

    return perpWallDist;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Raycasting Demo");
    window.setFramerateLimit(60);

    Player player;

    // ミニマップ用の設定
    const float miniMapScale = 10.0f;
    const float miniMapOffsetX = 10.0f;
    const float miniMapOffsetY = 10.0f;

    while (window.isOpen()) {
        // イベント処理 (SFML 3.0 スタイル)
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        // === 入力処理 (ウィンドウにフォーカスがある時のみ) ===
        if (window.hasFocus()) {
            // 回転
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
                player.angle -= player.rotSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
                player.angle += player.rotSpeed;
            }

            // 移動（衝突判定付き）
            float newX = player.x;
            float newY = player.y;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up)) {
                newX += cos(player.angle) * player.moveSpeed;
                newY += sin(player.angle) * player.moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down)) {
                newX -= cos(player.angle) * player.moveSpeed;
                newY -= sin(player.angle) * player.moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) {
                newX += cos(player.angle - PI / 2) * player.moveSpeed;
                newY += sin(player.angle - PI / 2) * player.moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) {
                newX += cos(player.angle + PI / 2) * player.moveSpeed;
                newY += sin(player.angle + PI / 2) * player.moveSpeed;
            }

            // 衝突判定（壁に入らないようにする）
            float margin = 0.2f;
            if (worldMap[static_cast<int>(player.y)][static_cast<int>(newX + margin)] == 0 &&
                worldMap[static_cast<int>(player.y)][static_cast<int>(newX - margin)] == 0) {
                player.x = newX;
            }
            if (worldMap[static_cast<int>(newY + margin)][static_cast<int>(player.x)] == 0 &&
                worldMap[static_cast<int>(newY - margin)][static_cast<int>(player.x)] == 0) {
                player.y = newY;
            }
        }

        // === 描画 ===
        window.clear();

        // 天井
        sf::RectangleShape ceiling({static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) / 2});
        ceiling.setFillColor(sf::Color(50, 50, 80));
        window.draw(ceiling);

        // 床
        sf::RectangleShape floor({static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) / 2});
        floor.setPosition({0, static_cast<float>(SCREEN_HEIGHT) / 2});
        floor.setFillColor(sf::Color(80, 60, 40));
        window.draw(floor);

        // レイキャスティングで壁を描画
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            // カメラ平面上の位置 (-1 〜 1)
            float cameraX = 2.0f * x / static_cast<float>(SCREEN_WIDTH) - 1.0f;
            float rayAngle = player.angle + atan(cameraX * tan(FOV / 2));

            // レイを飛ばして距離を取得
            float distance = castRay(player.x, player.y, rayAngle);

            // 魚眼補正
            float correctedDist = distance * cos(rayAngle - player.angle);

            // 壁の高さを計算
            int wallHeight = static_cast<int>(SCREEN_HEIGHT / correctedDist);
            if (wallHeight > SCREEN_HEIGHT) wallHeight = SCREEN_HEIGHT;

            int drawStart = (SCREEN_HEIGHT - wallHeight) / 2;

            // 距離に応じて色を暗くする（シェーディング）
            int shade = static_cast<int>(255 - std::min(correctedDist * 30.0f, 200.0f));
            if (shade < 55) shade = 55;

            // 壁を描画
            sf::RectangleShape wallLine({1.0f, static_cast<float>(wallHeight)});
            wallLine.setPosition({static_cast<float>(x), static_cast<float>(drawStart)});
            wallLine.setFillColor(sf::Color(static_cast<std::uint8_t>(shade),
                                            static_cast<std::uint8_t>(shade / 2),
                                            static_cast<std::uint8_t>(shade / 4)));
            window.draw(wallLine);
        }

        // === ミニマップ描画 ===
        // マップの背景
        sf::RectangleShape mapBg({MAP_WIDTH * miniMapScale, MAP_HEIGHT * miniMapScale});
        mapBg.setPosition({miniMapOffsetX, miniMapOffsetY});
        mapBg.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(mapBg);

        // 壁を描画
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                if (worldMap[y][x] == 1) {
                    sf::RectangleShape wall({miniMapScale - 1, miniMapScale - 1});
                    wall.setPosition({miniMapOffsetX + x * miniMapScale, miniMapOffsetY + y * miniMapScale});
                    wall.setFillColor(sf::Color(100, 100, 100));
                    window.draw(wall);
                }
            }
        }

        // プレイヤーを描画
        sf::CircleShape playerDot(3);
        playerDot.setFillColor(sf::Color::Red);
        playerDot.setPosition({
            miniMapOffsetX + player.x * miniMapScale - 3,
            miniMapOffsetY + player.y * miniMapScale - 3
        });
        window.draw(playerDot);

        // プレイヤーの向きを示す線
        std::array<sf::Vertex, 2> dirLine = {
            sf::Vertex{{
                miniMapOffsetX + player.x * miniMapScale,
                miniMapOffsetY + player.y * miniMapScale
            }, sf::Color::Yellow},
            sf::Vertex{{
                miniMapOffsetX + (player.x + cos(player.angle) * 1.5f) * miniMapScale,
                miniMapOffsetY + (player.y + sin(player.angle) * 1.5f) * miniMapScale
            }, sf::Color::Yellow}
        };
        window.draw(dirLine.data(), dirLine.size(), sf::PrimitiveType::Lines);

        window.display();
    }

    return 0;
}
