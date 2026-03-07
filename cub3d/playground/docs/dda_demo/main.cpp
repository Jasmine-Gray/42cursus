#include <SFML/Graphics.hpp>
#include <cmath>
#include <array>
#include <vector>
#include <cstdint>

constexpr int SCREEN_WIDTH = 1050;
constexpr int SCREEN_HEIGHT = 700;
constexpr int MAP_WIDTH = 7;
constexpr int MAP_HEIGHT = 7;
constexpr float CELL_SIZE = 70.0f;
constexpr float PI = 3.14159265f;

int worldMap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1}
};

struct Player {
    float x = 2.5f;
    float y = 3.5f;
    float angle = 0;  // 右向き
};

struct DDAStep {
    int mapX, mapY;
    float hitX, hitY;  // 境界との交点
    int side;          // 0=X境界, 1=Y境界
    bool isHit;
};

std::vector<DDAStep> runDDA(float px, float py, float angle) {
    std::vector<DDAStep> steps;

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

    // 開始点を追加
    steps.push_back({mapX, mapY, px, py, -1, false});

    int maxSteps = 30;
    while (steps.size() < maxSteps) {
        float hitX, hitY;
        int side;

        if (sideDistX < sideDistY) {
            hitX = px + sideDistX * rayDirX;
            hitY = py + sideDistX * rayDirY;
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        } else {
            hitX = px + sideDistY * rayDirX;
            hitY = py + sideDistY * rayDirY;
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }

        if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) break;

        bool hit = (worldMap[mapY][mapX] > 0);
        steps.push_back({mapX, mapY, hitX, hitY, side, hit});

        if (hit) break;
    }

    return steps;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "DDA Algorithm Demo - Interactive");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = font.openFromFile("/System/Library/Fonts/Helvetica.ttc");

    Player player;
    bool showStepByStep = false;
    int currentStep = 0;
    float stepTimer = 0;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scancode::Escape) window.close();
                if (key->scancode == sf::Keyboard::Scancode::Space) {
                    showStepByStep = !showStepByStep;
                    currentStep = 0;
                }
                if (key->scancode == sf::Keyboard::Scancode::R) {
                    currentStep = 0;
                }
            }
        }

        // プレイヤー移動
        if (window.hasFocus()) {
            float rotSpeed = 0.04f;
            float moveSpeed = 0.06f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) player.angle -= rotSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) player.angle += rotSpeed;

            float newX = player.x, newY = player.y;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up)) {
                newX += cos(player.angle) * moveSpeed;
                newY += sin(player.angle) * moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down)) {
                newX -= cos(player.angle) * moveSpeed;
                newY -= sin(player.angle) * moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) {
                newX += cos(player.angle - PI/2) * moveSpeed;
                newY += sin(player.angle - PI/2) * moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) {
                newX += cos(player.angle + PI/2) * moveSpeed;
                newY += sin(player.angle + PI/2) * moveSpeed;
            }

            float margin = 0.3f;
            if (newX > margin && newX < MAP_WIDTH - margin &&
                worldMap[(int)player.y][(int)newX] == 0) player.x = newX;
            if (newY > margin && newY < MAP_HEIGHT - margin &&
                worldMap[(int)newY][(int)player.x] == 0) player.y = newY;
        }

        // DDA計算
        std::vector<DDAStep> steps = runDDA(player.x, player.y, player.angle);

        // ステップアニメーション
        if (showStepByStep) {
            stepTimer += 1.0f / 60.0f;
            if (stepTimer > 0.3f) {
                stepTimer = 0;
                currentStep++;
                if (currentStep >= steps.size()) currentStep = 0;
            }
        } else {
            currentStep = steps.size() - 1;
        }

        window.clear(sf::Color(25, 25, 35));

        float mapOffsetX = 30;
        float mapOffsetY = 30;

        // === グリッドを描画 ===
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                sf::RectangleShape cell({CELL_SIZE - 2, CELL_SIZE - 2});
                cell.setPosition({mapOffsetX + x * CELL_SIZE + 1, mapOffsetY + y * CELL_SIZE + 1});

                if (worldMap[y][x] == 1) {
                    cell.setFillColor(sf::Color(80, 80, 100));
                } else {
                    cell.setFillColor(sf::Color(40, 40, 50));
                }
                window.draw(cell);
            }
        }

        // === グリッド線 ===
        for (int i = 0; i <= MAP_WIDTH; i++) {
            std::array<sf::Vertex, 2> line = {
                sf::Vertex{{mapOffsetX + i * CELL_SIZE, mapOffsetY}, sf::Color(60, 60, 70)},
                sf::Vertex{{mapOffsetX + i * CELL_SIZE, mapOffsetY + MAP_HEIGHT * CELL_SIZE}, sf::Color(60, 60, 70)}
            };
            window.draw(line.data(), 2, sf::PrimitiveType::Lines);
        }
        for (int i = 0; i <= MAP_HEIGHT; i++) {
            std::array<sf::Vertex, 2> line = {
                sf::Vertex{{mapOffsetX, mapOffsetY + i * CELL_SIZE}, sf::Color(60, 60, 70)},
                sf::Vertex{{mapOffsetX + MAP_WIDTH * CELL_SIZE, mapOffsetY + i * CELL_SIZE}, sf::Color(60, 60, 70)}
            };
            window.draw(line.data(), 2, sf::PrimitiveType::Lines);
        }

        // === DDAのステップを描画 ===
        int drawSteps = std::min(currentStep + 1, (int)steps.size());

        for (int i = 0; i < drawSteps; i++) {
            const auto& step = steps[i];

            // 通過したセルをハイライト
            sf::RectangleShape highlight({CELL_SIZE - 4, CELL_SIZE - 4});
            highlight.setPosition({mapOffsetX + step.mapX * CELL_SIZE + 2, mapOffsetY + step.mapY * CELL_SIZE + 2});

            if (step.isHit) {
                highlight.setFillColor(sf::Color(200, 60, 60, 180));  // 壁: 赤
            } else if (i == currentStep) {
                highlight.setFillColor(sf::Color(60, 200, 60, 180));  // 現在: 緑
            } else {
                highlight.setFillColor(sf::Color(60, 120, 200, 120)); // 通過: 青
            }
            window.draw(highlight);

            // 境界との交点を描画
            if (i > 0) {
                sf::CircleShape hitPoint(4);
                hitPoint.setFillColor(step.side == 0 ? sf::Color::Yellow : sf::Color::Cyan);
                hitPoint.setPosition({
                    mapOffsetX + step.hitX * CELL_SIZE - 4,
                    mapOffsetY + step.hitY * CELL_SIZE - 4
                });
                window.draw(hitPoint);
            }

            // ステップ番号
            if (fontLoaded) {
                sf::Text num(font, std::to_string(i), 12);
                num.setPosition({mapOffsetX + step.mapX * CELL_SIZE + 5, mapOffsetY + step.mapY * CELL_SIZE + 3});
                num.setFillColor(sf::Color::White);
                window.draw(num);
            }
        }

        // === レイを描画 ===
        if (drawSteps > 1) {
            const auto& lastStep = steps[drawSteps - 1];
            std::array<sf::Vertex, 2> ray = {
                sf::Vertex{{mapOffsetX + player.x * CELL_SIZE, mapOffsetY + player.y * CELL_SIZE}, sf::Color(255, 255, 0, 200)},
                sf::Vertex{{mapOffsetX + lastStep.hitX * CELL_SIZE, mapOffsetY + lastStep.hitY * CELL_SIZE}, sf::Color(255, 200, 0, 150)}
            };
            window.draw(ray.data(), 2, sf::PrimitiveType::Lines);
        }

        // === プレイヤー ===
        sf::CircleShape playerDot(8);
        playerDot.setFillColor(sf::Color::Red);
        playerDot.setPosition({mapOffsetX + player.x * CELL_SIZE - 8, mapOffsetY + player.y * CELL_SIZE - 8});
        window.draw(playerDot);

        // 視線方向
        float dirLen = 0.8f;
        std::array<sf::Vertex, 2> dirLine = {
            sf::Vertex{{mapOffsetX + player.x * CELL_SIZE, mapOffsetY + player.y * CELL_SIZE}, sf::Color::Red},
            sf::Vertex{{mapOffsetX + (player.x + cos(player.angle) * dirLen) * CELL_SIZE,
                        mapOffsetY + (player.y + sin(player.angle) * dirLen) * CELL_SIZE}, sf::Color::Red}
        };
        window.draw(dirLine.data(), 2, sf::PrimitiveType::Lines);

        // === 右側: 説明パネル ===
        float panelX = mapOffsetX + MAP_WIDTH * CELL_SIZE + 30;
        float panelY = 30;

        if (fontLoaded) {
            sf::Text title(font, "DDA Algorithm", 22);
            title.setPosition({panelX, panelY});
            title.setFillColor(sf::Color::White);
            window.draw(title);

            // 凡例
            std::vector<std::pair<std::string, sf::Color>> legend = {
                {"Blue = Checked cell", sf::Color(60, 120, 200)},
                {"Green = Current step", sf::Color(60, 200, 60)},
                {"Red = Wall hit!", sf::Color(200, 60, 60)},
                {"Yellow dot = X boundary", sf::Color::Yellow},
                {"Cyan dot = Y boundary", sf::Color::Cyan}
            };

            for (int i = 0; i < legend.size(); i++) {
                sf::RectangleShape box({15, 15});
                box.setPosition({panelX, panelY + 40 + i * 25.0f});
                box.setFillColor(legend[i].second);
                window.draw(box);

                sf::Text label(font, legend[i].first, 14);
                label.setPosition({panelX + 25, panelY + 38 + i * 25.0f});
                label.setFillColor(sf::Color(200, 200, 200));
                window.draw(label);
            }

            // 現在のステップ情報
            if (currentStep < steps.size()) {
                const auto& step = steps[currentStep];

                sf::Text stepTitle(font, "Current Step: " + std::to_string(currentStep), 16);
                stepTitle.setPosition({panelX, panelY + 190});
                stepTitle.setFillColor(sf::Color::Yellow);
                window.draw(stepTitle);

                std::string info = "Grid: (" + std::to_string(step.mapX) + ", " + std::to_string(step.mapY) + ")";
                sf::Text infoText(font, info, 14);
                infoText.setPosition({panelX, panelY + 215});
                infoText.setFillColor(sf::Color::Cyan);
                window.draw(infoText);

                if (step.side >= 0) {
                    std::string sideStr = (step.side == 0) ? "Crossed X boundary (vertical)" : "Crossed Y boundary (horizontal)";
                    sf::Text sideText(font, sideStr, 13);
                    sideText.setPosition({panelX, panelY + 240});
                    sideText.setFillColor(step.side == 0 ? sf::Color::Yellow : sf::Color::Cyan);
                    window.draw(sideText);
                }

                if (step.isHit) {
                    sf::Text hitText(font, "*** WALL HIT! ***", 16);
                    hitText.setPosition({panelX, panelY + 270});
                    hitText.setFillColor(sf::Color::Red);
                    window.draw(hitText);
                }
            }

            // アルゴリズムの説明
            sf::Text algoTitle(font, "How DDA works:", 15);
            algoTitle.setPosition({panelX, panelY + 310});
            algoTitle.setFillColor(sf::Color::White);
            window.draw(algoTitle);

            std::vector<std::string> algo = {
                "1. Start at player position",
                "2. Calculate distance to next",
                "   X and Y grid lines",
                "3. Move to the CLOSER one",
                "4. Check if new cell is wall",
                "5. Repeat until wall found"
            };

            for (int i = 0; i < algo.size(); i++) {
                sf::Text line(font, algo[i], 12);
                line.setPosition({panelX, panelY + 335.0f + i * 18});
                line.setFillColor(sf::Color(170, 170, 170));
                window.draw(line);
            }

            // 距離表示
            if (steps.size() > 1) {
                const auto& lastStep = steps.back();
                float dist = sqrt(pow(lastStep.hitX - player.x, 2) + pow(lastStep.hitY - player.y, 2));

                char buf[64];
                snprintf(buf, sizeof(buf), "Distance to wall: %.2f", dist);
                sf::Text distText(font, buf, 14);
                distText.setPosition({panelX, panelY + 450});
                distText.setFillColor(sf::Color::Green);
                window.draw(distText);
            }

            // 操作説明
            sf::Text controls(font, "Controls:", 15);
            controls.setPosition({panelX, panelY + 490});
            controls.setFillColor(sf::Color::White);
            window.draw(controls);

            std::vector<std::string> ctrlList = {
                "W/S/A/D or Arrows: Move",
                "Left/Right: Rotate",
                "SPACE: Toggle step-by-step",
                "R: Reset animation"
            };

            for (int i = 0; i < ctrlList.size(); i++) {
                sf::Text line(font, ctrlList[i], 12);
                line.setPosition({panelX, panelY + 515.0f + i * 18});
                line.setFillColor(sf::Color(150, 150, 150));
                window.draw(line);
            }

            // モード表示
            std::string modeStr = showStepByStep ? "Mode: Step-by-step (animated)" : "Mode: Real-time";
            sf::Text modeText(font, modeStr, 14);
            modeText.setPosition({panelX, panelY + 600});
            modeText.setFillColor(showStepByStep ? sf::Color::Yellow : sf::Color::Green);
            window.draw(modeText);
        }

        window.display();
    }

    return 0;
}
