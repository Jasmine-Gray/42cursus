#include "Intern.hpp"
#include <iostream>

Intern::Intern() {}

Intern::Intern(const Intern& other) {
    (void)other;
}

Intern& Intern::operator=(const Intern& other) {
    (void)other;
    return *this;
}

Intern::~Intern() {}

AForm* Intern::makeShrubbery(const std::string& target) const {
    return new ShrubberyCreationForm(target);
}

AForm* Intern::makeRobotomy(const std::string& target) const {
    return new RobotomyRequestForm(target);
}

AForm* Intern::makePresidential(const std::string& target) const {
    return new PresidentialPardonForm(target);
}

AForm* Intern::makeForm(const std::string& formName, const std::string& target) const {
    std::string formNames[] = {
        "shrubbery creation",
        "robotomy request",
        "presidential pardon"
    };

    AForm* (Intern::*formCreators[])(const std::string&) const = {
        &Intern::makeShrubbery,
        &Intern::makeRobotomy,
        &Intern::makePresidential
    };

    for (int i = 0; i < 3; ++i) {
        if (formName == formNames[i]) {
            std::cout << "Intern creates " << formName << std::endl;
            return (this->*formCreators[i])(target); 
        }
    }

    std::cerr << "Error: Intern cannot create form '" << formName << "' because it does not exist." << std::endl;
    return NULL;
}
