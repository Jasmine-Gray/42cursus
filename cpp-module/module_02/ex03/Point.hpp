#ifndef POINT_HPP
# define POINT_HPP

# include "Fixed.hpp"

class Point {
private:
    Fixed const x;
    Fixed const y;

public:
    // Orthodox Canonical Form
    Point();
    Point(const float x, const float y);
    Point(const Point& other);
    Point& operator=(const Point& other);
    ~Point();

    // ゲッター (BSPの計算用)
    Fixed const & getX(void) const;
    Fixed const & getY(void) const;
};

#endif
