#include "FragTrap.hpp"
#include <iostream>

FragTrap::FragTrap() : ClapTrap() {
    this->hitPoints = 100;
    this->energyPoints = 100;
    this->attackDamage = 30;
    std::cout << "FragTrap Default constructor called for " << this->name << std::endl;
}

FragTrap::FragTrap(const std::string& name) : ClapTrap(name) {
    this->hitPoints = 100;
    this->energyPoints = 100;
    this->attackDamage = 30;
    std::cout << "FragTrap Parameterized constructor called for " << this->name << std::endl;
}

FragTrap::FragTrap(const FragTrap& other) : ClapTrap(other) {
    std::cout << "FragTrap Copy constructor called" << std::endl;
}

FragTrap& FragTrap::operator=(const FragTrap& other) {
    std::cout << "FragTrap Copy assignment operator called" << std::endl;
    if (this != &other) {
        ClapTrap::operator=(other);
    }
    return *this;
}

FragTrap::~FragTrap() {
    std::cout << "FragTrap " << this->name << " destructor called" << std::endl;
}

void FragTrap::highFivesGuys(void) {
    if (this->hitPoints == 0) {
        std::cout << "FragTrap " << this->name << " is broken and can't high five..." << std::endl;
        return;
    }
    std::cout << "FragTrap " << this->name << " says: \"Hey guys! Give me a positive high-five! ✋\"" << std::endl;
}
