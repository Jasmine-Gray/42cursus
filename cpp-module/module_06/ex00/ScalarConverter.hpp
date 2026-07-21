#ifndef SCALARCONVERTER_HPP
#define SCALARCONVERTER_HPP

#include <string>
#include <iostream>

class ScalarConverter {
private:
    // ユーザーによるインスタンス化を防ぐため、すべてprivateに配置
    ScalarConverter();
    ScalarConverter(const ScalarConverter& src);
    ScalarConverter& operator=(const ScalarConverter& rhs);
    ~ScalarConverter();

public:
    // 課題で要求されている唯一の静的メソッド
    static void convert(const std::string& literal);
};

#endif
