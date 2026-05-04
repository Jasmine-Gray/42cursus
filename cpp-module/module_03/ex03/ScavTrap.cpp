#include "ScavTrap.hpp"

ScavTrap::ScavTrap() : ClapTrap() {
    this->hitPoints = 100;
    this->energyPoints = 50;
    this->attackDamage = 20;
    std::cout << "ScavTrap Default constructor called for " << this->name << std::endl;
}

ScavTrap::ScavTrap(const std::string& name) : ClapTrap(name) {
    this->hitPoints = 100;
    this->energyPoints = 50;
    this->attackDamage = 20;
    std::cout << "ScavTrap Parameterized constructor called for " << this->name << std::endl;
}

ScavTrap::ScavTrap(const ScavTrap& other) : ClapTrap(other) {
    std::cout << "ScavTrap Copy constructor called" << std::endl;
}

ScavTrap& ScavTrap::operator=(const ScavTrap& other) {
    std::cout << "ScavTrap Copy assignment operator called" << std::endl;
    if (this != &other) {
        ClapTrap::operator=(other);
    }
    return *this;
}

ScavTrap::~ScavTrap() {
    std::cout << "ScavTrap " << this->name << " destructor called" << std::endl;
}

void ScavTrap::attack(const std::string& target) {
    if (this->hitPoints == 0 || this->energyPoints == 0) {
        std::cout << "ScavTrap " << this->name << " cannot attack! (No HP or EP)" << std::endl;
        return;
    }
    this->energyPoints -= 1;
    std::cout << "ScavTrap " << this->name << " viciously attacks " << target 
              << ", causing " << this->attackDamage << " points of massive damage!" << std::endl;
}

void ScavTrap::guardGate() {
    if (this->hitPoints == 0) {
        std::cout << "ScavTrap " << this->name << " is destroyed and cannot enter Gate keeper mode." << std::endl;
        return;
    }
    std::cout << "ScavTrap " << this->name << " is now in Gate keeper mode!" << std::endl;
}
