#include "RobotomyRequestForm.hpp"
#include "Bureaucrat.hpp"
#include <cstdlib> 
#include <iostream>

RobotomyRequestForm::RobotomyRequestForm() : AForm("RobotomyRequestForm", 72, 45), target("Default") {}

RobotomyRequestForm::RobotomyRequestForm(const std::string& target) : AForm("RobotomyRequestForm", 72, 45), target(target) {}

RobotomyRequestForm::RobotomyRequestForm(const RobotomyRequestForm& other) : AForm(other), target(other.target) {}

RobotomyRequestForm& RobotomyRequestForm::operator=(const RobotomyRequestForm& other) {
    if (this != &other) {
        AForm::operator=(other);
        this->target = other.target;
    }
    return *this;
}

RobotomyRequestForm::~RobotomyRequestForm() {}

void RobotomyRequestForm::execute(Bureaucrat const & executor) const {
    this->checkExecutable(executor); // 実行可能かチェック

    std::cout << "* BZZZZZZZZ... VVRRRRRRR... *" << std::endl;
    // 50%の確率で成功/失敗
    if (std::rand() % 2 == 0) {
        std::cout << this->target << " has been robotomized successfully!" << std::endl;
    } else {
        std::cout << "The robotomy on " << this->target << " failed." << std::endl;
    }
}
