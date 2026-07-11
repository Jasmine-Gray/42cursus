#include "Bureaucrat.hpp"
#include <iostream>

int main() {
    std::cout << "--- Test 1: Normal instantiation ---" << std::endl;
    try {
        Bureaucrat john("John", 42);
        std::cout << john << std::endl;
        
        john.incrementGrade();
        std::cout << "After increment: " << john << std::endl;
        
        john.decrementGrade();
        std::cout << "After decrement: " << john << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 2: Grade too high on creation ---" << std::endl;
    try {
        Bureaucrat boss("Boss", 0); 
        std::cout << boss << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 3: Grade too low on creation ---" << std::endl;
    try {
        Bureaucrat lazy("Lazy", 151); 
        std::cout << lazy << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 4: Incrementing grade 1 ---" << std::endl;
    try {
        Bureaucrat top("Top", 1);
        std::cout << top << std::endl;
        top.incrementGrade(); 
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 5: Decrementing grade 150 ---" << std::endl;
    try {
        Bureaucrat bottom("Bottom", 150);
        std::cout << bottom << std::endl;
        bottom.decrementGrade();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
