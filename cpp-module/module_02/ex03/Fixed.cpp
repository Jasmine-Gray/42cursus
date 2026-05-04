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

bool Fixed::operator>(const Fixed& other) const { return this->value > other.getRawBits(); }
bool Fixed::operator<(const Fixed& other) const { return this->value < other.getRawBits(); }
bool Fixed::operator>=(const Fixed& other) const { return this->value >= other.getRawBits(); }
bool Fixed::operator<=(const Fixed& other) const { return this->value <= other.getRawBits(); }
bool Fixed::operator==(const Fixed& other) const { return this->value == other.getRawBits(); }
bool Fixed::operator!=(const Fixed& other) const { return this->value != other.getRawBits(); }

//  算術演算子
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

Fixed Fixed::operator*(const Fixed& other) const {
    return Fixed(this->toFloat() * other.toFloat());
}

Fixed Fixed::operator/(const Fixed& other) const {
    return Fixed(this->toFloat() / other.toFloat());
}

// 増減演算子 
Fixed& Fixed::operator++() {
    this->value++; // 最小単位 ε だけ増やす
    return *this;
}

Fixed Fixed::operator++(int) {
    Fixed temp(*this); 
    this->value++;     
    return temp;       
}

Fixed& Fixed::operator--() {
    this->value--;
    return *this;
}

Fixed Fixed::operator--(int) {
    Fixed temp(*this);
    this->value--;
    return temp;
}

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
