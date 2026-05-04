#include "Point.hpp"

// ベクトルAB と ベクトルAP の外積 = (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x)
static Fixed crossProduct(Point const a, Point const b, Point const p) {
    return (b.getX() - a.getX()) * (p.getY() - a.getY()) - 
           (b.getY() - a.getY()) * (p.getX() - a.getX());
}

bool bsp(Point const a, Point const b, Point const c, Point const point) {
    Fixed cp1 = crossProduct(a, b, point);
    Fixed cp2 = crossProduct(b, c, point);
    Fixed cp3 = crossProduct(c, a, point);

    Fixed zero(0);

    if (cp1 == zero || cp2 == zero || cp3 == zero)
        return false;

    bool isPositive = (cp1 > zero) && (cp2 > zero) && (cp3 > zero);
    bool isNegative = (cp1 < zero) && (cp2 < zero) && (cp3 < zero);

    return isPositive || isNegative;
}
