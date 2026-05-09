#include "AMateria.hpp"
#include "ICharacter.hpp"

AMateria::AMateria() : type("unknown") {}

AMateria::AMateria(std::string const & type) : type(type) {}

AMateria::AMateria(const AMateria& other) : type(other.type) {}

AMateria& AMateria::operator=(const AMateria& other) {
    if (this != &other) {
        // Subjectの指示: "While assigning a Materia to another, copying the type doesn't make sense."
        // そのため type はコピーしません。
        (void)other;
    }
    return *this;
}

AMateria::~AMateria() {}

std::string const & AMateria::getType() const {
    return this->type;
}

void AMateria::use(ICharacter& target) {
    std::cout << "* uses " << this->type << " on " << target.getName() << " *" << std::endl;
}
