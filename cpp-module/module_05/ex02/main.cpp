#include "Bureaucrat.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>

int main() {
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    std::cout << "--- 1. Shrubbery Creation Form Tests ---" << std::endl;
    try {
        Bureaucrat highRank("HighRank", 1);
        Bureaucrat lowRank("LowRank", 140); // can sign(145) but cannot execute(137)
        ShrubberyCreationForm tree("Home");

        highRank.executeForm(tree); // Should fail because not signed
        
        lowRank.signForm(tree); // Success
        lowRank.executeForm(tree); // Should fail because grade too low

        highRank.executeForm(tree); // Success, creates Home_shrubbery
    } catch (const std::exception& e) {
        std::cerr << "Unexpected Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- 2. Robotomy Request Form Tests ---" << std::endl;
    try {
        Bureaucrat midRank("MidRank", 50); // can sign(72) but cannot execute(45)
        Bureaucrat boss("Boss", 1);
        RobotomyRequestForm robot("Bender");

        midRank.signForm(robot); // Success
        midRank.executeForm(robot); // Fails (too low to execute)

        boss.executeForm(robot); // Success (50% chance outcome)
        boss.executeForm(robot); // Try again for rng test
    } catch (const std::exception& e) {
        std::cerr << "Unexpected Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- 3. Presidential Pardon Form Tests ---" << std::endl;
    try {
        Bureaucrat president("President", 1);
        Bureaucrat nobody("Nobody", 150);
        PresidentialPardonForm pardon("Arthur Dent");

        nobody.signForm(pardon); // Fails

        president.signForm(pardon); // Success
        president.executeForm(pardon); // Success
    } catch (const std::exception& e) {
        std::cerr << "Unexpected Exception: " << e.what() << std::endl;
    }

    return 0;
}
