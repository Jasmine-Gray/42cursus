#include <SFML/Graphics.hpp>
#include <cmath>
#include <array>
#include <cstdint>

constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 700;
constexpr int MAP_WIDTH = 10;
constexpr int MAP_HEIGHT = 10;
constexpr float PI = 3.14159265f;
constexpr float FOV = 60.0f * PI / 180.0f;

int worldMap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},  // 長い廊下
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player {
    float x = 2.5f;
    float y = 5.0f;
    float angle = 0;  // 右向き（長い廊下が見える）
};

struct RayResult {
    float distance;
    int side;  // 0=X面(縦), 1=Y面(横)
    float wallX;  // 壁のどこに当たったか (0.0-1.0)
};

RayResult castRay(float px, float py, float angle) {
    float rayDirX = cos(angle);
    float rayDirY = sin(angle);

    int mapX = (int)px;
    int mapY = (int)py;

    float deltaDistX = (rayDirX == 0) ? 1e30f : std::abs(1.0f / rayDirX);
    float deltaDistY = (rayDirY == 0) ? 1e30f : std::abs(1.0f / rayDirY);

    float sideDistX, sideDistY;
    int stepX, stepY;

    if (rayDirX < 0) { stepX = -1; sideDistX = (px - mapX) * deltaDistX; }
    else { stepX = 1; sideDistX = (mapX + 1.0f - px) * deltaDistX; }
    if (rayDirY < 0) { stepY = -1; sideDistY = (py - mapY) * deltaDistY; }
    else { stepY = 1; sideDistY = (mapY + 1.0f - py) * deltaDistY; }

    int side = 0;
    while (true) {
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }
        if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) break;
        if (worldMap[mapY][mapX] > 0) break;
    }

    float perpDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);

    // 壁のどこに当たったか計算
    float wallX;
    if (side == 0) {
        wallX = py + perpDist * rayDirY;
    } else {
        wallX = px + perpDist * rayDirX;
    }
    wallX -= floor(wallX);

    return {perpDist, side, wallX};
}

int main() {
    sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Depth Perception Demo");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = font.openFromFile("/System/Library/Fonts/Helvetica.ttc");

    Player player;

    // 表示モード: 0=シェーディングなし, 1=距離シェーディング, 2=面シェーディング, 3=両方
    int mode = 3;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scancode::Escape) window.close();
                if (key->scancode == sf::Keyboard::Scancode::Num1) mode = 0;
                if (key->scancode == sf::Keyboard::Scancode::Num2) mode = 1;
                if (key->scancode == sf::Keyboard::Scancode::Num3) mode = 2;
                if (key->scancode == sf::Keyboard::Scancode::Num4) mode = 3;
            }
        }

        // 入力
        if (window.hasFocus()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) player.angle -= 0.04f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) player.angle += 0.04f;

            float newX = player.x, newY = player.y;
            float speed = 0.08f;

            // W または 上キー で前進
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up)) {
                newX += cos(player.angle) * speed;
                newY += sin(player.angle) * speed;
            }
            // S または 下キー で後退
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down)) {
                newX -= cos(player.angle) * speed;
                newY -= sin(player.angle) * speed;
            }

            float margin = 0.2f;
            if (newX > margin && newX < MAP_WIDTH - margin &&
                worldMap[(int)player.y][(int)(newX + margin)] == 0 &&
                worldMap[(int)player.y][(int)(newX - margin)] == 0) {
                player.x = newX;
            }
            if (newY > margin && newY < MAP_HEIGHT - margin &&
                worldMap[(int)(newY + margin)][(int)player.x] == 0 &&
                worldMap[(int)(newY - margin)][(int)player.x] == 0) {
                player.y = newY;
            }
        }

        window.clear(sf::Color(20, 20, 30));

        // 3Dビュー領域
        float viewWidth = 600;
        float viewHeight = 400;
        float viewX = 20;
        float viewY = 20;

        // 天井
        sf::RectangleShape ceiling({viewWidth, viewHeight / 2});
        ceiling.setPosition({viewX, viewY});
        ceiling.setFillColor(sf::Color(40, 40, 60));
        window.draw(ceiling);

        // 床
        sf::RectangleShape floorRect({viewWidth, viewHeight / 2});
        floorRect.setPosition({viewX, viewY + viewHeight / 2});
        floorRect.setFillColor(sf::Color(60, 50, 40));
        window.draw(floorRect);

        // 壁を描画
        for (int x = 0; x < (int)viewWidth; x++) {
            float cameraX = 2.0f * x / viewWidth - 1.0f;
            float rayAngle = player.angle + atan(cameraX * tan(FOV / 2));

            RayResult ray = castRay(player.x, player.y, rayAngle);
            float correctedDist = ray.distance * cos(rayAngle - player.angle);

            float wallHeight = viewHeight / correctedDist;
            if (wallHeight > viewHeight) wallHeight = viewHeight;
            float drawY = viewY + (viewHeight - wallHeight) / 2;

            // 色を決定
            int r, g, b;

            switch (mode) {
                case 0:  // シェーディングなし
                    r = g = b = 180;
                    break;

                case 1:  // 距離シェーディングのみ
                    {
                        int shade = (int)(255 - correctedDist * 35);
                        if (shade < 40) shade = 40;
                        r = g = b = shade;
                    }
                    break;

                case 2:  // 面シェーディングのみ
                    if (ray.side == 0) {
                        r = 200; g = 150; b = 100;  // X面: 明るい
                    } else {
                        r = 140; g = 100; b = 70;   // Y面: 暗い
                    }
                    break;

                case 3:  // 両方
                default:
                    {
                        float shade = 1.0f - correctedDist * 0.12f;
                        if (shade < 0.2f) shade = 0.2f;

                        if (ray.side == 0) {
                            r = (int)(220 * shade);
                            g = (int)(180 * shade);
                            b = (int)(140 * shade);
                        } else {
                            r = (int)(160 * shade);
                            g = (int)(120 * shade);
                            b = (int)(90 * shade);
                        }
                    }
                    break;
            }

            sf::RectangleShape wall({1, wallHeight});
            wall.setPosition({viewX + x, drawY});
            wall.setFillColor(sf::Color(r, g, b));
            window.draw(wall);
        }

        // 枠線
        sf::RectangleShape frame({viewWidth, viewHeight});
        frame.setPosition({viewX, viewY});
        frame.setFillColor(sf::Color::Transparent);
        frame.setOutlineColor(sf::Color::White);
        frame.setOutlineThickness(2);
        window.draw(frame);

        // ミニマップ
        float mapX = 650;
        float mapY = 20;
        float cellSize = 25;

        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                sf::RectangleShape cell({cellSize - 1, cellSize - 1});
                cell.setPosition({mapX + x * cellSize, mapY + y * cellSize});
                cell.setFillColor(worldMap[y][x] ? sf::Color(100, 100, 100) : sf::Color(40, 40, 50));
                window.draw(cell);
            }
        }

        sf::CircleShape playerDot(4);
        playerDot.setFillColor(sf::Color::Red);
        playerDot.setPosition({mapX + player.x * cellSize - 4, mapY + player.y * cellSize - 4});
        window.draw(playerDot);

        // 説明
        if (fontLoaded) {
            std::vector<std::string> modeNames = {
                "1: No Shading (flat)",
                "2: Distance Shading (far = dark)",
                "3: Side Shading (X/Y face)",
                "4: Both (realistic)"
            };

            sf::Text title(font, "Depth Perception Methods", 18);
            title.setPosition({viewX, viewY + viewHeight + 20});
            title.setFillColor(sf::Color::White);
            window.draw(title);

            for (int i = 0; i < 4; i++) {
                sf::Text opt(font, modeNames[i], 14);
                opt.setPosition({viewX, viewY + viewHeight + 50.0f + i * 25});
                opt.setFillColor(i == mode ? sf::Color::Yellow : sf::Color(150, 150, 150));
                window.draw(opt);
            }

            // 現在のモードの説明
            std::vector<std::string> descriptions = {
                "All walls same color - no depth perception",
                "Farther walls are darker - creates fog effect",
                "X-facing and Y-facing walls have different brightness",
                "Combination gives most realistic depth"
            };

            sf::Text desc(font, descriptions[mode], 14);
            desc.setPosition({viewX, viewY + viewHeight + 160});
            desc.setFillColor(sf::Color::Cyan);
            window.draw(desc);

            // コード例
            std::vector<std::string> codeExamples = {
                "shade = 180;  // constant",
                "shade = 255 - distance * 35;",
                "shade = (side == 0) ? 200 : 140;",
                "shade = base_color * (1 - dist * 0.12) * side_factor;"
            };

            sf::Text code(font, "Code: " + codeExamples[mode], 12);
            code.setPosition({viewX, viewY + viewHeight + 185});
            code.setFillColor(sf::Color(100, 255, 100));
            window.draw(code);

            sf::Text controls(font, "W/S: Move | Arrows: Rotate | 1-4: Change mode", 14);
            controls.setPosition({viewX, SCREEN_HEIGHT - 30});
            controls.setFillColor(sf::Color::White);
            window.draw(controls);
        }

        window.display();
    }

    return 0;
}
