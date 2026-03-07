#include <SFML/Graphics.hpp>
#include <cmath>
#include <array>
#include <cstdint>
#include <string>

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;
constexpr float PI = 3.14159265f;
constexpr float FOV = 60.0f * PI / 180.0f;

// 壁までの垂直距離
constexpr float WALL_DISTANCE = 5.0f;

int main() {
    sf::RenderWindow window(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Fisheye Correction Demo");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = font.openFromFile("/System/Library/Fonts/Helvetica.ttc");

    bool showCorrection = true;  // 補正ON/OFF

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                    showCorrection = !showCorrection;
                }
            }
        }

        window.clear(sf::Color(30, 30, 40));

        // ============================================
        // 左側: 2D トップダウンビュー
        // ============================================
        float view2dX = 50;
        float view2dY = 50;
        float scale = 40.0f;

        // プレイヤー位置
        float playerX = view2dX + 5 * scale;
        float playerY = view2dY + 8 * scale;

        // 壁（水平線）
        float wallY = view2dY + (8 - WALL_DISTANCE) * scale;
        sf::RectangleShape wallLine({300, 4});
        wallLine.setPosition({view2dX + 50, wallY});
        wallLine.setFillColor(sf::Color(150, 150, 150));
        window.draw(wallLine);

        // 視線方向（中央のレイ）の補助線
        std::array<sf::Vertex, 2> centerLine = {
            sf::Vertex{{playerX, playerY}, sf::Color(100, 100, 100)},
            sf::Vertex{{playerX, wallY}, sf::Color(100, 100, 100)}
        };
        window.draw(centerLine.data(), 2, sf::PrimitiveType::Lines);

        // 垂直距離を示す線（緑）
        std::array<sf::Vertex, 2> perpLine = {
            sf::Vertex{{playerX + 10, playerY}, sf::Color::Green},
            sf::Vertex{{playerX + 10, wallY}, sf::Color::Green}
        };
        window.draw(perpLine.data(), 2, sf::PrimitiveType::Lines);

        // 複数のレイを描画
        int numRays = 7;
        std::vector<float> rayDistances;
        std::vector<float> correctedDistances;

        for (int i = 0; i < numRays; i++) {
            float t = (float)i / (numRays - 1);  // 0 to 1
            float angleOffset = (t - 0.5f) * FOV;  // -30° to +30°

            // レイが壁に当たる点を計算
            float rayDist = WALL_DISTANCE / cos(angleOffset);
            float hitX = playerX + sin(angleOffset) * rayDist * scale;
            float hitY = wallY;

            rayDistances.push_back(rayDist);
            correctedDistances.push_back(WALL_DISTANCE);  // 補正後は全部同じ

            // レイを描画
            sf::Color rayColor;
            if (i == numRays / 2) {
                rayColor = sf::Color::Yellow;  // 中央は黄色
            } else {
                rayColor = sf::Color(255, 150, 50, 200);  // 端はオレンジ
            }

            std::array<sf::Vertex, 2> ray = {
                sf::Vertex{{playerX, playerY}, rayColor},
                sf::Vertex{{hitX, hitY}, rayColor}
            };
            window.draw(ray.data(), 2, sf::PrimitiveType::Lines);

            // 距離を表示
            if (fontLoaded && (i == 0 || i == numRays / 2 || i == numRays - 1)) {
                char buf[32];
                snprintf(buf, sizeof(buf), "%.2f", rayDist);
                sf::Text distText(font, buf, 12);
                distText.setPosition({hitX - 15, hitY - 20});
                distText.setFillColor(sf::Color::White);
                window.draw(distText);
            }
        }

        // プレイヤー
        sf::CircleShape playerDot(8);
        playerDot.setFillColor(sf::Color::Red);
        playerDot.setPosition({playerX - 8, playerY - 8});
        window.draw(playerDot);

        // 2Dビューのラベル
        if (fontLoaded) {
            sf::Text label(font, "2D Top-Down View", 16);
            label.setPosition({view2dX, view2dY - 30});
            label.setFillColor(sf::Color::White);
            window.draw(label);

            sf::Text wallLabel(font, "Wall (horizontal)", 12);
            wallLabel.setPosition({view2dX + 50, wallY - 20});
            wallLabel.setFillColor(sf::Color(150, 150, 150));
            window.draw(wallLabel);

            sf::Text perpLabel(font, "Perpendicular distance = 5.0", 12);
            perpLabel.setPosition({playerX + 20, (playerY + wallY) / 2});
            perpLabel.setFillColor(sf::Color::Green);
            window.draw(perpLabel);
        }

        // ============================================
        // 右側: 3D ビュー（補正あり/なし比較）
        // ============================================
        float view3dX = 500;
        float view3dY = 50;
        float view3dWidth = 450;
        float view3dHeight = 200;

        // 背景
        sf::RectangleShape bg3d({view3dWidth, view3dHeight});
        bg3d.setPosition({view3dX, view3dY});
        bg3d.setFillColor(sf::Color(20, 20, 30));
        bg3d.setOutlineColor(sf::Color::White);
        bg3d.setOutlineThickness(1);
        window.draw(bg3d);

        // 天井
        sf::RectangleShape ceiling({view3dWidth, view3dHeight / 2});
        ceiling.setPosition({view3dX, view3dY});
        ceiling.setFillColor(sf::Color(50, 50, 80));
        window.draw(ceiling);

        // 床
        sf::RectangleShape floor({view3dWidth, view3dHeight / 2});
        floor.setPosition({view3dX, view3dY + view3dHeight / 2});
        floor.setFillColor(sf::Color(80, 60, 40));
        window.draw(floor);

        // 壁を描画
        int wallSegments = 100;
        for (int i = 0; i < wallSegments; i++) {
            float t = (float)i / wallSegments;
            float angleOffset = (t - 0.5f) * FOV;

            float rayDist = WALL_DISTANCE / cos(angleOffset);
            float dist = showCorrection ? WALL_DISTANCE : rayDist;

            float wallHeight = view3dHeight * 0.8f / dist;
            if (wallHeight > view3dHeight) wallHeight = view3dHeight;

            float drawY = view3dY + (view3dHeight - wallHeight) / 2;

            int shade = (int)(200 - dist * 20);
            if (shade < 50) shade = 50;

            sf::RectangleShape wallSeg({view3dWidth / wallSegments + 1, wallHeight});
            wallSeg.setPosition({view3dX + i * view3dWidth / wallSegments, drawY});
            wallSeg.setFillColor(sf::Color(shade, shade, shade));
            window.draw(wallSeg);
        }

        // 3Dビューのラベル
        if (fontLoaded) {
            std::string label3d = showCorrection ?
                "3D View: With Correction (Press SPACE to toggle)" :
                "3D View: NO Correction - FISHEYE! (Press SPACE to toggle)";
            sf::Text label(font, label3d, 14);
            label.setPosition({view3dX, view3dY - 25});
            label.setFillColor(showCorrection ? sf::Color::Green : sf::Color::Red);
            window.draw(label);
        }

        // ============================================
        // 下部: 距離の比較表
        // ============================================
        float tableY = 320;

        if (fontLoaded) {
            sf::Text title(font, "Distance Comparison:", 16);
            title.setPosition({50, tableY});
            title.setFillColor(sf::Color::White);
            window.draw(title);

            // ヘッダー
            sf::Text header(font, "Ray        Actual Dist    cos(angle)    Corrected Dist", 14);
            header.setPosition({50, tableY + 30});
            header.setFillColor(sf::Color(200, 200, 200));
            window.draw(header);

            // データ行
            std::vector<std::pair<std::string, float>> rays = {
                {"Left -30", -30.0f},
                {"Mid-L -15", -15.0f},
                {"Center 0", 0.0f},
                {"Mid-R +15", 15.0f},
                {"Right +30", 30.0f}
            };

            for (int i = 0; i < rays.size(); i++) {
                float angleDeg = rays[i].second;
                float angleRad = angleDeg * PI / 180.0f;
                float actualDist = WALL_DISTANCE / cos(angleRad);
                float cosVal = cos(angleRad);
                float corrected = actualDist * cosVal;

                char buf[128];
                snprintf(buf, sizeof(buf), "%-12s  %.3f         %.3f         %.3f",
                    rays[i].first.c_str(), actualDist, cosVal, corrected);

                sf::Text row(font, buf, 13);
                row.setPosition({50, tableY + 55.0f + i * 22});
                row.setFillColor(sf::Color::Cyan);
                window.draw(row);
            }

            // 計算式
            sf::Text formula(font, "Formula: correctedDist = actualDist * cos(rayAngle - playerAngle)", 14);
            formula.setPosition({50, tableY + 180});
            formula.setFillColor(sf::Color::Yellow);
            window.draw(formula);

            // 解説
            std::vector<std::string> explanation = {
                "Why correction is needed:",
                "- All rays hit the SAME wall at distance 5.0 (perpendicular)",
                "- But diagonal rays travel LONGER distance to reach the wall",
                "- Without correction: diagonal rays make wall look farther = curved",
                "- With correction: all rays report distance 5.0 = straight wall"
            };

            for (int i = 0; i < explanation.size(); i++) {
                sf::Text line(font, explanation[i], 13);
                line.setPosition({500, tableY + 30.0f + i * 20});
                line.setFillColor(sf::Color(180, 180, 180));
                window.draw(line);
            }
        }

        // 操作説明
        if (fontLoaded) {
            sf::Text controls(font, "SPACE: Toggle correction ON/OFF | ESC: Quit", 14);
            controls.setPosition({50, SCREEN_HEIGHT - 30});
            controls.setFillColor(sf::Color::White);
            window.draw(controls);
        }

        window.display();
    }

    return 0;
}
