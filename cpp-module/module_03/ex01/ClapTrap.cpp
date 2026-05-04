#include "ClapTrap.hpp"

ClapTrap::ClapTrap() : name("Default"), hitPoints(10), energyPoints(10), attackDamage(0) {
    std::cout << "ClapTrap Default constructor called" << std::endl;
}

ClapTrap::ClapTrap(const std::string& name) : name(name), hitPoints(10), energyPoints(10), attackDamage(0) {
    std::cout << "ClapTrap " << this->name << " constructor called" << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap& other) : name(other.name), hitPoints(other.hitPoints), energyPoints(other.energyPoints), attackDamage(other.attackDamage) {
    std::cout << "ClapTrap Copy constructor called" << std::endl;
}

ClapTrap& ClapTrap::operator=(const ClapTrap& other) {
    std::cout << "ClapTrap Copy assignment operator called" << std::endl;
    if (this != &other) {
        this->name = other.name;
        this->hitPoints = other.hitPoints;
        this->energyPoints = other.energyPoints;
        this->attackDamage = other.attackDamage;
    }
    return *this;
}

ClapTrap::~ClapTrap() {
    std::cout << "ClapTrap " << this->name << " destructor called" << std::endl;
}

void ClapTrap::attack(const std::string& target) {
    if (this->hitPoints == 0 || this->energyPoints == 0) {
        std::cout << "ClapTrap " << this->name << " cannot attack! (No hit points or energy points left)" << std::endl;
        return;
    }
    this->energyPoints -= 1;
    std::cout << "ClapTrap " << this->name << " attacks " << target 
              << ", causing " << this->attackDamage << " points of damage!" << std::endl;
}

void ClapTrap::takeDamage(unsigned int amount) {
    if (this->hitPoints < amount) {
        this->hitPoints = 0;
    } else {
        this->hitPoints -= amount;
    }
    std::cout << "ClapTrap " << this->name << " took " << amount 
              << " points of damage! (Current HP: " << this->hitPoints << ")" << std::endl;
}

void ClapTrap::beRepaired(unsigned int amount) {
    if (this->hitPoints == 0 || this->energyPoints == 0) {
        std::cout << "ClapTrap " << this->name << " cannot be repaired! (No hit points or energy points left)" << std::endl;
        return;
    }
    this->energyPoints -= 1;
    this->hitPoints += amount;
    std::cout << "ClapTrap " << this->name << " repairs itself, recovering " << amount 
              << " hit points! (Current HP: " << this->hitPoints << ")" << std::endl;
}
