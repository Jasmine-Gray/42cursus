#ifndef AFORM_HPP
#define AFORM_HPP

#include <iostream>
#include <string>
#include <exception>

class Bureaucrat;

class AForm {
private:
    const std::string name;
    bool isSigned;
    const int gradeToSign;
    const int gradeToExecute;

public:
    AForm();
    AForm(const AForm& other);
    AForm& operator=(const AForm& other);
    virtual ~AForm();

    AForm(const std::string& name, int gradeToSign, int gradeToExecute);

    const std::string& getName() const;
    bool getIsSigned() const;
    int getGradeToSign() const;
    int getGradeToExecute() const;

    void beSigned(const Bureaucrat& bureaucrat);

    // 純粋仮想関数にしてAFormを抽象クラスにする
    virtual void execute(Bureaucrat const & executor) const = 0;

    // 実行可能かどうかのチェックを基底クラスにまとめる（エレガントな方法）
    void checkExecutable(Bureaucrat const & executor) const;

    class GradeTooHighException : public std::exception {
    public:
        virtual const char* what() const throw();
    };

    class GradeTooLowException : public std::exception {
    public:
        virtual const char* what() const throw();
    };

    class FormNotSignedException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
};

std::ostream& operator<<(std::ostream& os, const AForm& form);

#endif
