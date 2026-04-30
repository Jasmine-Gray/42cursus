#include "Point.hpp"

// 外積（2Dクロス積）を計算する補助関数
// ベクトルAB と ベクトルAP の外積 = (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x)
static Fixed crossProduct(Point const a, Point const b, Point const p) {
    return (b.getX() - a.getX()) * (p.getY() - a.getY()) - 
           (b.getY() - a.getY()) * (p.getX() - a.getX());
}

bool bsp(Point const a, Point const b, Point const c, Point const point) {
    Fixed cp1 = crossProduct(a, b, point);
    Fixed cp2 = crossProduct(b, c, point);
    Fixed cp3 = crossProduct(c, a, point);

    // 0のオブジェクトを作成（比較用）
    Fixed zero(0);

    // 点が頂点または辺上にある場合 (外積が0になる)
    if (cp1 == zero || cp2 == zero || cp3 == zero)
        return false;

    // 全て正、または全て負であれば内側
    bool isPositive = (cp1 > zero) && (cp2 > zero) && (cp3 > zero);
    bool isNegative = (cp1 < zero) && (cp2 < zero) && (cp3 < zero);

    return isPositive || isNegative;
}
