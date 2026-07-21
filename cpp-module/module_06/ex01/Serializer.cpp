#include "Serializer.hpp"

// ========================================================================= //
// Orthodox Canonical Form (Private)
// ========================================================================= //
Serializer::Serializer() {}

Serializer::Serializer(const Serializer& src) {
    (void)src;
}

Serializer& Serializer::operator=(const Serializer& rhs) {
    (void)rhs;
    return *this;
}

Serializer::~Serializer() {}

// ========================================================================= //
// Static Methods
// ========================================================================= //
uintptr_t Serializer::serialize(Data* ptr) {
    // ポインタを符号なし整数型に変換
    return reinterpret_cast<uintptr_t>(ptr);
}

Data* Serializer::deserialize(uintptr_t raw) {
    // 符号なし整数型をポインタに変換
    return reinterpret_cast<Data*>(raw);
}
