#include "Point.hpp"

// デフォルトコンストラクタ（x, y を 0 に初期化）
Point::Point() : x(0), y(0) {}

// float を受け取るコンストラクタ
Point::Point(const float x, const float y) : x(x), y(y) {}

// コピーコンストラクタ（初期化子リストで const メンバにセット）
Point::Point(const Point& other) : x(other.x), y(other.y) {}

// コピー代入演算子
Point& Point::operator=(const Point& other) {
    if (this != &other) {
        // x と y は const なので通常は代入できません。
        // Orthodox Canonical Form を満たしつつ値をコピーするため、
        // const_cast を使って一時的に const を外して代入します。
        const_cast<Fixed&>(this->x) = other.getX();
        const_cast<Fixed&>(this->y) = other.getY();
    }
    return *this;
}

// デストラクタ
Point::~Point() {}

// ゲッター
Fixed const & Point::getX(void) const {
    return this->x;
}

Fixed const & Point::getY(void) const {
    return this->y;
}
