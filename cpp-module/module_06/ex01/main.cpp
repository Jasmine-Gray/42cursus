#include "Serializer.hpp"
#include "Data.hpp"
#include <iostream>

int main() {
    // 1. 非空のDataオブジェクトを生成し、初期化
    Data myData;
    myData.id = 42;
    myData.name = "Test Data Structure";

    std::cout << "--- Original Data ---" << std::endl;
    std::cout << "Address : " << &myData << std::endl;
    std::cout << "ID      : " << myData.id << std::endl;
    std::cout << "Name    : " << myData.name << std::endl;
    std::cout << std::endl;

    // 2. serialize() を使用してアドレスを uintptr_t に変換
    uintptr_t serialized = Serializer::serialize(&myData);
    
    // uintptr_t の値を16進数で出力して確認（アドレス表現に近い形）
    std::cout << "--- Serialized ---" << std::endl;
    std::cout << "Raw int : 0x" << std::hex << serialized << std::dec << std::endl;
    std::cout << std::endl;

    // 3. deserialize() を使用して uintptr_t を Data* に復元
    Data* deserialized = Serializer::deserialize(serialized);

    std::cout << "--- Deserialized ---" << std::endl;
    std::cout << "Address : " << deserialized << std::endl;
    std::cout << "ID      : " << deserialized->id << std::endl;
    std::cout << "Name    : " << deserialized->name << std::endl;
    std::cout << std::endl;

    // 4. 復元されたポインタが元のポインタと等しいか比較
    std::cout << "--- Verification ---" << std::endl;
    if (deserialized == &myData) {
        std::cout << "Result  : SUCCESS! Pointers match." << std::endl;
    } else {
        std::cout << "Result  : ERROR! Pointers do not match." << std::endl;
    }

    return 0;
}
