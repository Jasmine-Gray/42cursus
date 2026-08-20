#include "Span.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>

int main() {
    std::cout << "--- 1. Subject Example Test ---" << std::endl;
    try {
        Span sp = Span(5);
        sp.addNumber(6);
        sp.addNumber(3);
        sp.addNumber(17);
        sp.addNumber(9);
        sp.addNumber(11);
        std::cout << "Shortest span: " << sp.shortestSpan() << std::endl; // 期待値: 2
        std::cout << "Longest span: " << sp.longestSpan() << std::endl;   // 期待値: 14
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "\n--- 2. Range of Iterators Test (10,000+ elements) ---" << std::endl;
    try {
        Span largeSp(15000);
        std::vector<int> tempVector;
        for (int i = 0; i < 15000; ++i) {
            tempVector.push_back(i * 3); // 0, 3, 6, 9...
        }
        
        // イテレータを使って一括追加
        largeSp.addNumbers(tempVector.begin(), tempVector.end());
        
        std::cout << "Successfully added 15,000 numbers." << std::endl;
        std::cout << "Shortest span: " << largeSp.shortestSpan() << " (Expected: 3)" << std::endl;
        std::cout << "Longest span: " << largeSp.longestSpan() << " (Expected: 44997)" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "\n--- 3. Exception Test ---" << std::endl;
    try {
        Span smallSp(1);
        smallSp.addNumber(42);
        std::cout << "Attempting to get shortest span with only 1 element..." << std::endl;
        std::cout << smallSp.shortestSpan() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
