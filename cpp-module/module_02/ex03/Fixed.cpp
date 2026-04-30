#include "Fixed.hpp"

Fixed::Fixed() : value(0) {
    std::cout << "Default constructor called" << std::endl;
}

Fixed::Fixed(const Fixed& other) {
    std::cout << "Copy constructor called" << std::endl;
    this->value = other.getRawBits();
}

Fixed& Fixed::operator=(const Fixed& other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other) {
        this->value = other.getRawBits();
    }
    return *this;
}

Fixed::~Fixed() {
    std::cout << "Destructor called" << std::endl;
}

Fixed::Fixed(const int n) {
    std::cout << "Int constructor called" << std::endl;
    this->value = n << fractionalBits;
}

Fixed::Fixed(const float f) {
    std::cout << "Float constructor called" << std::endl;
    this->value = roundf(f * (1 << fractionalBits));
}

float Fixed::toFloat(void) const {
    return (float)this->value / (1 << fractionalBits);
}

int Fixed::toInt(void) const {
    return this->value >> fractionalBits;
}

int Fixed::getRawBits(void) const {
    return this->value;
}

void Fixed::setRawBits(int const raw) {
    this->value = raw;
}

std::ostream &operator<<(std::ostream &out, const Fixed &fixed) {
    out << fixed.toFloat();
    return out;
}

// --- 比較演算子 ---
bool Fixed::operator>(const Fixed& other) const { return this->value > other.getRawBits(); }
bool Fixed::operator<(const Fixed& other) const { return this->value < other.getRawBits(); }
bool Fixed::operator>=(const Fixed& other) const { return this->value >= other.getRawBits(); }
bool Fixed::operator<=(const Fixed& other) const { return this->value <= other.getRawBits(); }
bool Fixed::operator==(const Fixed& other) const { return this->value == other.getRawBits(); }
bool Fixed::operator!=(const Fixed& other) const { return this->value != other.getRawBits(); }

// --- 算術演算子 ---
// 足し算・引き算は生のビット値を直接操作して新しいオブジェクトを作る
Fixed Fixed::operator+(const Fixed& other) const {
    Fixed result;
    result.setRawBits(this->value + other.getRawBits());
    return result;
}

Fixed Fixed::operator-(const Fixed& other) const {
    Fixed result;
    result.setRawBits(this->value - other.getRawBits());
    return result;
}

// 掛け算・割り算は一度floatに戻して計算し、再度Fixed化する
Fixed Fixed::operator*(const Fixed& other) const {
    return Fixed(this->toFloat() * other.toFloat());
}

Fixed Fixed::operator/(const Fixed& other) const {
    // 0除算はプログラムがクラッシュしても許容される仕様
    return Fixed(this->toFloat() / other.toFloat());
}

// --- 増減演算子 ---
// 前置インクリメント (++a): 自身を増やしてから返す
Fixed& Fixed::operator++() {
    this->value++; // 最小単位 ε だけ増やす
    return *this;
}

// 後置インクリメント (a++): 増やす前の状態をコピーして返し、自身は増やす
Fixed Fixed::operator++(int) {
    Fixed temp(*this); // 現在の状態を保存
    this->value++;     // 自身は増やす
    return temp;       // 保存しておいた古い状態を返す
}

// 前置デクリメント (--a)
Fixed& Fixed::operator--() {
    this->value--;
    return *this;
}

// 後置デクリメント (a--)
Fixed Fixed::operator--(int) {
    Fixed temp(*this);
    this->value--;
    return temp;
}

// --- 静的メンバ関数 ---
Fixed& Fixed::min(Fixed& a, Fixed& b) {
    return (a < b) ? a : b;
}

const Fixed& Fixed::min(const Fixed& a, const Fixed& b) {
    return (a < b) ? a : b;
}

Fixed& Fixed::max(Fixed& a, Fixed& b) {
    return (a > b) ? a : b;
}

const Fixed& Fixed::max(const Fixed& a, const Fixed& b) {
    return (a > b) ? a : b;
}
