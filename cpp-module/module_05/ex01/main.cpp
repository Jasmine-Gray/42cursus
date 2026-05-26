#include "Bureaucrat.hpp"
#include "Form.hpp"
#include <iostream>

int main() {
    std::cout << "--- Test 1: Valid Form Creation ---" << std::endl;
    try {
        Form taxForm("Tax Form", 50, 20);
        std::cout << taxForm << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 2: Invalid Form Creation (Grade Too High) ---" << std::endl;
    try {
        Form topSecret("Top Secret", 0, 50); // 0 is too high
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 3: Invalid Form Creation (Grade Too Low) ---" << std::endl;
    try {
        Form garbage("Garbage", 150, 151); // 151 is too low
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 4: Bureaucrat signing a form successfully ---" << std::endl;
    try {
        Bureaucrat manager("Manager", 40);
        Form contract("Contract", 50, 20); // requires 50 to sign
        
        std::cout << manager << std::endl;
        std::cout << contract << std::endl;
        
        manager.signForm(contract);
        
        std::cout << "After signing attempt: " << contract << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Test 5: Bureaucrat failing to sign a form ---" << std::endl;
    try {
        Bureaucrat intern("Intern", 100);
        Form treaty("Peace Treaty", 10, 5); // requires 10 to sign
        
        std::cout << intern << std::endl;
        std::cout << treaty << std::endl;
        
        intern.signForm(treaty); // Intern ranks 100, needs 10
        
        std::cout << "After signing attempt: " << treaty << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
