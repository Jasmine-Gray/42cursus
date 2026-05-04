#include "DiamondTrap.hpp"
#include <iostream>

DiamondTrap::DiamondTrap() : ClapTrap("Default_clap_name"), ScavTrap(), FragTrap(), name("Default") {
    this->hitPoints = 100;  
    this->energyPoints = 50;
    this->attackDamage = 30;
    std::cout << "DiamondTrap Default constructor called" << std::endl;
}

DiamondTrap::DiamondTrap(const std::string& name) 
    : ClapTrap(name + "_clap_name"), ScavTrap(name), FragTrap(name), name(name) {
    
    this->hitPoints = 100;   
    this->energyPoints = 50;
    this->attackDamage = 30;
    
    std::cout << "DiamondTrap Parameterized constructor called for " << this->name << std::endl;
}

DiamondTrap::DiamondTrap(const DiamondTrap& other) 
    : ClapTrap(other), ScavTrap(other), FragTrap(other), name(other.name) {
    std::cout << "DiamondTrap Copy constructor called" << std::endl;
}

DiamondTrap& DiamondTrap::operator=(const DiamondTrap& other) {
    std::cout << "DiamondTrap Copy assignment operator called" << std::endl;
    if (this != &other) {
        ClapTrap::operator=(other);
        this->name = other.name;
        this->hitPoints = other.hitPoints;
        this->energyPoints = other.energyPoints;
        this->attackDamage = other.attackDamage;
    }
    return *this;
}

DiamondTrap::~DiamondTrap() {
    std::cout << "DiamondTrap " << this->name << " destructor called" << std::endl;
}

void DiamondTrap::whoAmI() {
    if (this->hitPoints == 0) {
        std::cout << "DiamondTrap " << this->name << " is broken and can't speak..." << std::endl;
        return;
    }
    std::cout << "I am DiamondTrap " << this->name 
              << ", but my ClapTrap name is " << ClapTrap::name << "!" << std::endl;
}
