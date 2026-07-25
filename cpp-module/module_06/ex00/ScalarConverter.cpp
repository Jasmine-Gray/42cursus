#include "ScalarConverter.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <cmath>
#include <cctype>

ScalarConverter::ScalarConverter() {}

ScalarConverter::ScalarConverter(const ScalarConverter& src) {
    (void)src;
}

ScalarConverter& ScalarConverter::operator=(const ScalarConverter& rhs) {
    (void)rhs;
    return *this;
}

ScalarConverter::~ScalarConverter() {}

void ScalarConverter::convert(const std::string& literal) {
    if (literal == "nan" || literal == "nanf") {
        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        std::cout << "float: nanf\n";
        std::cout << "double: nan\n";
        return;
    }
    if (literal == "+inf" || literal == "+inff" || literal == "inf" || literal == "inff") {
        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        std::cout << "float: +inff\n";
        std::cout << "double: +inf\n";
        return;
    }
    if (literal == "-inf" || literal == "-inff") {
        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        std::cout << "float: -inff\n";
        std::cout << "double: -inf\n";
        return;
    }

    double val = 0.0;
    
    if (literal.length() == 1 && !std::isdigit(literal[0])) {
        val = static_cast<double>(literal[0]);
    } else {
        std::string s = literal;
        
        if (s.length() > 1 && s[s.length() - 1] == 'f') {
            s.erase(s.length() - 1);
        }

        std::stringstream ss(s);
        ss >> val;

        if (ss.fail() || !ss.eof()) {
            std::cout << "char: impossible\n"
                      << "int: impossible\n"
                      << "float: impossible\n"
                      << "double: impossible\n";
            return;
        }
    }


    // --- char型 ---
    if (val < std::numeric_limits<char>::min() || val > std::numeric_limits<char>::max() || std::isnan(val) || std::isinf(val)) {
        std::cout << "char: impossible\n";
    } else {
        char c = static_cast<char>(val);
        if (std::isprint(c)) {
            std::cout << "char: '" << c << "'\n";
        } else {
            std::cout << "char: Non displayable\n";
        }
    }

    // --- int型 ---
    if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max() || std::isnan(val) || std::isinf(val)) {
        std::cout << "int: impossible\n";
    } else {
        std::cout << "int: " << static_cast<int>(val) << "\n";
    }

    // --- float型 ---
    float fval = static_cast<float>(val);
    std::cout << "float: " << fval;
    if (val == std::floor(val) && !std::isinf(val) && !std::isnan(val)) {
        std::cout << ".0";
    }
    std::cout << "f\n";

    // --- double型 ---
    std::cout << "double: " << val;
    if (val == std::floor(val) && !std::isinf(val) && !std::isnan(val)) {
        std::cout << ".0";
    }
    std::cout << "\n";
}
