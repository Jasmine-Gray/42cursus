#include "easyfind.hpp"
#include <iostream>
#include <vector>
#include <list>

int main() {
    std::cout << "--- Testing with std::vector ---" << std::endl;
    std::vector<int> vec;
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i * 10); // 0, 10, 20, 30, 40
    }

    try {
        std::cout << "Searching for 20: ";
        std::vector<int>::iterator it1 = easyfind(vec, 20);
        std::cout << "Found " << *it1 << std::endl;

        std::cout << "Searching for 42: ";
        std::vector<int>::iterator it2 = easyfind(vec, 42);
        std::cout << "Found " << *it2 << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "\n--- Testing with std::list ---" << std::endl;
    std::list<int> lst;
    for (int i = 0; i < 5; ++i) {
        lst.push_back(i * 100); // 0, 100, 200, 300, 400
    }

    try {
        std::cout << "Searching for 300: ";
        std::list<int>::iterator it3 = easyfind(lst, 300);
        std::cout << "Found " << *it3 << std::endl;

        std::cout << "Searching for 999: ";
        std::list<int>::iterator it4 = easyfind(lst, 999);
        std::cout << "Found " << *it4 << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
