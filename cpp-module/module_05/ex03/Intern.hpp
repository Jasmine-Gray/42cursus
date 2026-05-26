#ifndef INTERN_HPP
#define INTERN_HPP

#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include <string>

class Intern {
private:
    // 各フォームを生成して返すprivateメンバ関数
    AForm* makeShrubbery(const std::string& target) const;
    AForm* makeRobotomy(const std::string& target) const;
    AForm* makePresidential(const std::string& target) const;

public:
    // Orthodox Canonical Form
    Intern();
    Intern(const Intern& other);
    Intern& operator=(const Intern& other);
    ~Intern();

    // フォーム生成関数
    AForm* makeForm(const std::string& formName, const std::string& target) const;
};

#endif
