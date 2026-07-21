#include "ScalarConverter.hpp"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        std::cerr << "Usage: ./convert <literal>" << std::endl;
        return 1;
    }

    // コマンドライン引数を std::string に変換（RAIIによりスコープを抜けると安全に破棄されます）
    std::string literal = argv[1];

    // ScalarConverterクラスはインスタンス化不可であるため、静的メソッドを直接呼び出す
    ScalarConverter::convert(literal);

    return 0;
}
