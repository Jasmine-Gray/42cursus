#include <iostream>
#include <string>
#include "iter.hpp"

template <typename T>
void printElement(T& elem) {
    std::cout << elem << " ";
}

template <typename T>
void printConstElement(const T& elem) {
    std::cout << elem << " ";
}

void increment(int& n) {
    n++;
}

int main(void) {
    int intArray[] = {1, 2, 3, 4, 5};
    const int constIntArray[] = {10, 20, 30, 40, 50};
    std::string strArray[] = {"Hello", "42", "World"};

    std::cout << "--- intArray (original) ---" << std::endl;
    ::iter(intArray, 5, printElement<int>);
    std::cout << std::endl << std::endl;

    std::cout << "--- intArray (after increment) ---" << std::endl;
    ::iter(intArray, 5, increment);
    ::iter(intArray, 5, printConstElement<int>); 
    std::cout << std::endl << std::endl;

    std::cout << "--- constIntArray ---" << std::endl;
    ::iter(constIntArray, 5, printConstElement<int>);
    std::cout << std::endl << std::endl;

    std::cout << "--- strArray ---" << std::endl;
    ::iter(strArray, 3, printConstElement<std::string>);
    std::cout << std::endl;

    return 0;
}
