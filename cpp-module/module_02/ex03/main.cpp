#include <iostream>
#include "Point.hpp"

bool bsp(Point const a, Point const b, Point const c, Point const point);

int main() {
    Point a(0.0f, 0.0f);
    Point b(5.0f, 0.0f);
    Point c(0.0f, 5.0f);

    Point p1(1.0f, 1.0f);
    Point p2(5.0f, 5.0f);
    Point p3(2.5f, 0.0f);

    std::cout << "p1 (inside): " << (bsp(a, b, c, p1) ? "True" : "False") << std::endl;
    std::cout << "p2 (outside): " << (bsp(a, b, c, p2) ? "True" : "False") << std::endl;
    std::cout << "p3 (edge): " << (bsp(a, b, c, p3) ? "True" : "False") << std::endl;

    return 0;
}
