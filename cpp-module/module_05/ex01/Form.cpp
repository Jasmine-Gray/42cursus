#include "Form.hpp"
#include "Bureaucrat.hpp"

Form::Form() : name("Default Form"), isSigned(false), gradeToSign(150), gradeToExecute(150) {}

Form::Form(const Form& other) 
    : name(other.name), isSigned(other.isSigned), gradeToSign(other.gradeToSign), gradeToExecute(other.gradeToExecute) {}

Form& Form::operator=(const Form& other) {
    if (this != &other) {
        this->isSigned = other.isSigned;
    }
    return *this;
}

Form::~Form() {}

Form::Form(const std::string& name, int gradeToSign, int gradeToExecute) 
    : name(name), isSigned(false), gradeToSign(gradeToSign), gradeToExecute(gradeToExecute) {
    if (gradeToSign < 1 || gradeToExecute < 1) {
        throw Form::GradeTooHighException();
    } else if (gradeToSign > 150 || gradeToExecute > 150) {
        throw Form::GradeTooLowException();
    }
}

const std::string& Form::getName() const { return this->name; }
bool Form::getIsSigned() const { return this->isSigned; }
int Form::getGradeToSign() const { return this->gradeToSign; }
int Form::getGradeToExecute() const { return this->gradeToExecute; }

void Form::beSigned(const Bureaucrat& bureaucrat) {
    // bureaucratのランクの数字が、gradeToSign「より大きい」場合はランクが「低い」ことを意味します
    if (bureaucrat.getGrade() > this->gradeToSign) {
        throw Form::GradeTooLowException();
    }
    this->isSigned = true;
}

const char* Form::GradeTooHighException::what() const throw() {
    return "Form grade requirement is too high!";
}

const char* Form::GradeTooLowException::what() const throw() {
    return "Form grade requirement is too low!";
}

// Insertion operator overload
std::ostream& operator<<(std::ostream& os, const Form& form) {
    os << "Form: " << form.getName() 
       << ", Status: " << (form.getIsSigned() ? "Signed" : "Not Signed")
       << ", Grade required to sign: " << form.getGradeToSign()
       << ", Grade required to execute: " << form.getGradeToExecute();
    return os;
}
