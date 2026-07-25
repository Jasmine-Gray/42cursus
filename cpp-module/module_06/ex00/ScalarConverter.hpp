#ifndef SCALARCONVERTER_HPP
#define SCALARCONVERTER_HPP

#include <string>
#include <iostream>

class ScalarConverter {
private:
    ScalarConverter();
    ScalarConverter(const ScalarConverter& src);
    ScalarConverter& operator=(const ScalarConverter& rhs);
    ~ScalarConverter();

public:
    static void convert(const std::string& literal);
};

#endif
