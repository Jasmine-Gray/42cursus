#include "Point.hpp"

Point::Point() : x(0), y(0) {}

Point::Point(const float x, const float y) : x(x), y(y) {}

Point::Point(const Point& other) : x(other.x), y(other.y) {}

Point& Point::operator=(const Point& other) {
    if (this != &other) {
        const_cast<Fixed&>(this->x) = other.getX();
        const_cast<Fixed&>(this->y) = other.getY();
    }
    return *this;
}

Point::~Point() {}

Fixed const & Point::getX(void) const {
    return this->x;
}

Fixed const & Point::getY(void) const {
    return this->y;
}
