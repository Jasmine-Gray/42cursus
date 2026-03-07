#include "map.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

void Map::draw(sf::RenderTarget &target) {

  if (grid.empty()) {
    return;
  }

  sf::RectangleShape background(sf::Vector2f((float)grid[0].size() * cellsize,
                                             (float)grid.size() * cellsize));

  background.setFillColor(sf::Color::White);

  target.draw(background);

  sf::RectangleShape cell(sf::Vector2f(cellsize * 0.95f, cellsize * 0.95f));
}
