#include "Intern.hpp"
#include <iostream>

// Orthodox Canonical Form
Intern::Intern() {}

Intern::Intern(const Intern& other) {
    (void)other; // メンバ変数がないため、未使用警告を回避
}

Intern& Intern::operator=(const Intern& other) {
    (void)other;
    return *this;
}

Intern::~Intern() {}

// Private member functions for creating specific forms
AForm* Intern::makeShrubbery(const std::string& target) const {
    return new ShrubberyCreationForm(target);
}

AForm* Intern::makeRobotomy(const std::string& target) const {
    return new RobotomyRequestForm(target);
}

AForm* Intern::makePresidential(const std::string& target) const {
    return new PresidentialPardonForm(target);
}

// makeForm function using arrays to avoid excessive if/else
AForm* Intern::makeForm(const std::string& formName, const std::string& target) const {
    // フォーム名の配列
    std::string formNames[] = {
        "shrubbery creation",
        "robotomy request",
        "presidential pardon"
    };

    // メンバ関数ポインタの配列
    AForm* (Intern::*formCreators[])(const std::string&) const = {
        &Intern::makeShrubbery,
        &Intern::makeRobotomy,
        &Intern::makePresidential
    };

    // ループで一致するものを探す
    for (int i = 0; i < 3; ++i) {
        if (formName == formNames[i]) {
            std::cout << "Intern creates " << formName << std::endl;
            return (this->*formCreators[i])(target); // 一致した関数を実行して返す
        }
    }

    // 存在しないフォーム名の場合のエラーメッセージ
    std::cerr << "Error: Intern cannot create form '" << formName << "' because it does not exist." << std::endl;
    return NULL;
}
