#include <iostream>
#include <string>
#include "Array.hpp"

#define MAX_VAL 750

int main(int, char**)
{
    std::cout << "--- 1. Testing Default Constructor ---" << std::endl;
    Array<int> emptyArray;
    std::cout << "emptyArray size: " << emptyArray.size() << std::endl;
    try {
        emptyArray[0] = 1; // ここで例外が発生するはず
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    std::cout << "\n--- 2. Testing Parameterized Constructor ---" << std::endl;
    Array<int> numbers(MAX_VAL);
    int* mirror = new int[MAX_VAL];
    srand(time(NULL));
    for (int i = 0; i < MAX_VAL; i++) {
        const int value = rand();
        numbers[i] = value;
        mirror[i] = value;
    }
    std::cout << "numbers size: " << numbers.size() << std::endl;

    std::cout << "\n--- 3. Testing Deep Copy (Copy Constructor & Assignment) ---" << std::endl;
    Array<int> tmp = numbers;
    Array<int> test(tmp);

    // コピー先を変更して、元の配列に影響がないか確認
    tmp[0] = -1;
    test[0] = -2;
    std::cout << "numbers[0] (should be original): " << numbers[0] << std::endl;
    std::cout << "tmp[0] (should be -1): " << tmp[0] << std::endl;
    std::cout << "test[0] (should be -2): " << test[0] << std::endl;

    for (int i = 0; i < MAX_VAL; i++) {
        if (mirror[i] != numbers[i]) {
            std::cerr << "didn't save the same value!!" << std::endl;
            return 1;
        }
    }

    std::cout << "\n--- 4. Testing Out of Bounds Exception ---" << std::endl;
    try {
        numbers[-2] = 0;
    } catch(const std::exception& e) {
        std::cerr << "-2 index: " << e.what() << '\n';
    }
    try {
        numbers[MAX_VAL] = 0;
    } catch(const std::exception& e) {
        std::cerr << "MAX_VAL index: " << e.what() << '\n';
    }

    std::cout << "\n--- 5. Testing Array of Complex Types (std::string) ---" << std::endl;
    Array<std::string> strArray(3);
    strArray[0] = "Hello";
    strArray[1] = "42";
    strArray[2] = "Tokyo";
    for (unsigned int i = 0; i < strArray.size(); i++) {
        std::cout << "strArray[" << i << "]: " << strArray[i] << std::endl;
    }

    delete[] mirror;
    return 0;
}
