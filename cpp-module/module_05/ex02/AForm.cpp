#include "AForm.hpp"
#include "Bureaucrat.hpp"

// Default Constructor
AForm::AForm() : name("Default AForm"), isSigned(false), gradeToSign(150), gradeToExecute(150) {}

// Copy Constructor
AForm::AForm(const AForm& other) 
    : name(other.name), isSigned(other.isSigned), gradeToSign(other.gradeToSign), gradeToExecute(other.gradeToExecute) {}

// Copy Assignment Operator
AForm& AForm::operator=(const AForm& other) {
    if (this != &other) {
        this->isSigned = other.isSigned;
    }
    return *this;
}

// Destructor
AForm::~AForm() {}

// Parameterized Constructor
AForm::AForm(const std::string& name, int gradeToSign, int gradeToExecute) 
    : name(name), isSigned(false), gradeToSign(gradeToSign), gradeToExecute(gradeToExecute) {
    if (gradeToSign < 1 || gradeToExecute < 1) {
        throw AForm::GradeTooHighException();
    } else if (gradeToSign > 150 || gradeToExecute > 150) {
        throw AForm::GradeTooLowException();
    }
}

// Getters
const std::string& AForm::getName() const { return this->name; }
bool AForm::getIsSigned() const { return this->isSigned; }
int AForm::getGradeToSign() const { return this->gradeToSign; }
int AForm::getGradeToExecute() const { return this->gradeToExecute; }

// Member functions
void AForm::beSigned(const Bureaucrat& bureaucrat) {
    if (bureaucrat.getGrade() > this->gradeToSign) {
        throw AForm::GradeTooLowException();
    }
    this->isSigned = true;
}

void AForm::checkExecutable(Bureaucrat const & executor) const {
    if (!this->isSigned) {
        throw AForm::FormNotSignedException();
    }
    if (executor.getGrade() > this->gradeToExecute) {
        throw AForm::GradeTooLowException();
    }
}

// Exceptions
const char* AForm::GradeTooHighException::what() const throw() {
    return "AForm grade requirement is too high!";
}

const char* AForm::GradeTooLowException::what() const throw() {
    return "AForm grade requirement is too low!";
}

const char* AForm::FormNotSignedException::what() const throw() {
    return "AForm is not signed!";
}

// Insertion operator overload
std::ostream& operator<<(std::ostream& os, const AForm& form) {
    os << "AForm: " << form.getName() 
       << ", Status: " << (form.getIsSigned() ? "Signed" : "Not Signed")
       << ", Grade required to sign: " << form.getGradeToSign()
       << ", Grade required to execute: " << form.getGradeToExecute();
    return os;
}
