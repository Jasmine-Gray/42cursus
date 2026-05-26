#include "Bureaucrat.hpp"
#include <iostream>

int main() {
    std::cout << "--- Test 1: Normal instantiation ---" << std::endl;
    try {
        Bureaucrat john("John", 42);
        std::cout << john << std::endl;
        
        john.incrementGrade(); // 42 -> 41
        std::cout << "After increment: " << john << std::endl;
        
        john.decrementGrade(); // 41 -> 42
        std::cout << "After decrement: " << john << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 2: Grade too high on creation ---" << std::endl;
    try {
        Bureaucrat boss("Boss", 0); // Invalid (Too high)
        std::cout << boss << std::endl; // Should not reach here
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 3: Grade too low on creation ---" << std::endl;
    try {
        Bureaucrat lazy("Lazy", 151); // Invalid (Too low)
        std::cout << lazy << std::endl; // Should not reach here
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 4: Incrementing grade 1 ---" << std::endl;
    try {
        Bureaucrat top("Top", 1);
        std::cout << top << std::endl;
        top.incrementGrade(); // Should throw GradeTooHighException
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 5: Decrementing grade 150 ---" << std::endl;
    try {
        Bureaucrat bottom("Bottom", 150);
        std::cout << bottom << std::endl;
        bottom.decrementGrade(); // Should throw GradeTooLowException
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
