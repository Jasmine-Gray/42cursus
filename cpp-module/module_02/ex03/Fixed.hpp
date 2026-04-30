#ifndef FIXED_HPP
# define FIXED_HPP

# include <iostream>
# include <cmath>

class Fixed {
public:
    // Orthodox Canonical Form
    Fixed();
    Fixed(const Fixed& other);
    Fixed& operator=(const Fixed& other);
    ~Fixed();

    // コンストラクタ
    Fixed(const int n);
    Fixed(const float f);

    // 変換関数
    float toFloat(void) const;
    int toInt(void) const;

    // ゲッター・セッター
    int getRawBits(void) const;
    void setRawBits(int const raw);

    // 1. 比較演算子
    bool operator>(const Fixed& other) const;
    bool operator<(const Fixed& other) const;
    bool operator>=(const Fixed& other) const;
    bool operator<=(const Fixed& other) const;
    bool operator==(const Fixed& other) const;
    bool operator!=(const Fixed& other) const;

    // 2. 算術演算子
    Fixed operator+(const Fixed& other) const;
    Fixed operator-(const Fixed& other) const;
    Fixed operator*(const Fixed& other) const;
    Fixed operator/(const Fixed& other) const;

    // 3. 増減演算子
    Fixed& operator++();    // 前置インクリメント (++a)
    Fixed operator++(int);  // 後置インクリメント (a++)
    Fixed& operator--();    // 前置デクリメント (--a)
    Fixed operator--(int);  // 後置デクリメント (a--)

    // 4. 静的メンバ関数
    static Fixed& min(Fixed& a, Fixed& b);
    static const Fixed& min(const Fixed& a, const Fixed& b);
    static Fixed& max(Fixed& a, Fixed& b);
    static const Fixed& max(const Fixed& a, const Fixed& b);

private:
    int value; // フィードバック遵守
    static const int fractionalBits = 8;
};

std::ostream &operator<<(std::ostream &out, const Fixed &fixed);

#endif
