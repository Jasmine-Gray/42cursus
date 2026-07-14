#ifndef INTERN_HPP
#define INTERN_HPP

#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include <string>

class Intern {
private:
    AForm* makeShrubbery(const std::string& target) const;
    AForm* makeRobotomy(const std::string& target) const;
    AForm* makePresidential(const std::string& target) const;

public:
    Intern();
    Intern(const Intern& other);
    Intern& operator=(const Intern& other);
    ~Intern();

    AForm* makeForm(const std::string& formName, const std::string& target) const;
};

#endif
