#ifndef _MAP_HPP
#define _MAP_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Map {
public:
  void draw(sf::RenderTarget &target);

private:
  std::vector<std::vector<int>> grid;
  float cellsize;
};

#endif // !_MAP_HPP
