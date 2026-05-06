#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"
#include "DiamondTrap.hpp"
#include <iostream>

int main() {
    std::cout << "========== ClapTrap Tests ==========" << std::endl;
    {
        ClapTrap b("Bob");
        b.attack("Target_A");
        b.takeDamage(5);
        b.beRepaired(3);
    }

    std::cout << "\n========== ScavTrap Tests ==========" << std::endl;
    {
        ScavTrap scav("Scavvy");
        scav.attack("Enemy_B");
        scav.takeDamage(20);
        scav.beRepaired(10);
        scav.guardGate();
    }

    std::cout << "\n========== FragTrap Tests ==========" << std::endl;
    {
        FragTrap frag("Fraggy");
        frag.attack("Enemy_C");
        frag.takeDamage(50);
        frag.beRepaired(20);
        frag.highFivesGuys();
    }

    std::cout << "\n========== DiamondTrap Tests ==========" << std::endl;
    {
        DiamondTrap diamond("Dia");

        diamond.attack("Enemy_D");

        diamond.takeDamage(30);
        diamond.beRepaired(10);

        diamond.guardGate();      
        diamond.highFivesGuys();  

        diamond.whoAmI();

    }

    std::cout << "\n========== End of All Tests ==========" << std::endl;
    return 0;
}
