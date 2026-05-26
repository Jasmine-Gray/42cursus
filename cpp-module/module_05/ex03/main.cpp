#include "Bureaucrat.hpp"
#include "Intern.hpp"
#include <iostream>

int main() {
    std::cout << "--- Intern Form Creation Tests ---" << std::endl;
    
    Intern someRandomIntern;
    AForm* form1;
    AForm* form2;
    AForm* form3;
    AForm* unknownForm;

    // 1. Shrubbery Creation Form
    form1 = someRandomIntern.makeForm("shrubbery creation", "Garden");
    
    // 2. Robotomy Request Form
    form2 = someRandomIntern.makeForm("robotomy request", "Bender");
    
    // 3. Presidential Pardon Form
    form3 = someRandomIntern.makeForm("presidential pardon", "Arthur Dent");
    
    // 4. 不明なフォーム（エラーメッセージが出力され、NULLが返る）
    unknownForm = someRandomIntern.makeForm("coffee making request", "Boss");

    std::cout << "\n--- Execution Tests ---" << std::endl;
    
    Bureaucrat boss("Boss", 1);
    
    if (form1) {
        boss.signForm(*form1);
        boss.executeForm(*form1);
    }
    if (form2) {
        boss.signForm(*form2);
        boss.executeForm(*form2);
    }
    if (form3) {
        boss.signForm(*form3);
        boss.executeForm(*form3);
    }
    
    // unknownFormがもし作成されていれば（実際はNULLですが）処理する記述を追加
    if (unknownForm) {
        boss.signForm(*unknownForm);
        boss.executeForm(*unknownForm);
    }

    // 動的確保されたオブジェクトを delete します
    delete form1;
    delete form2;
    delete form3;
    delete unknownForm; // これを追加することで変数が「使用された」ことになり、エラーが消えます

    return 0;
}
