#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <stdint.h> 
#include "Data.hpp"

class Serializer {
private:
    // OCF: ユーザーによるインスタンス化を完全に防ぐためprivateに配置
    Serializer();
    Serializer(const Serializer& src);
    Serializer& operator=(const Serializer& rhs);
    ~Serializer();

public:
    // 課題で要求されている静的メソッド
    static uintptr_t serialize(Data* ptr);
    static Data* deserialize(uintptr_t raw);
};

#endif
