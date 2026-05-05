#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"
#include <iostream>

int main() {
    std::cout << "\n========== ClapTrap Constructor & OCF Tests ==========" << std::endl;
    {
        ClapTrap a;
        ClapTrap b("Bob");
        ClapTrap c(b);
        ClapTrap d("Dave");
        d = b;
    }

    std::cout << "\n========== ClapTrap Basic Actions ==========" << std::endl;
    {
        ClapTrap b("Bob");
        b.attack("Target_A");
        b.takeDamage(5);
        b.beRepaired(3);

        std::cout << "\n--- Overkill & HP 0 Test ---" << std::endl;
        b.takeDamage(100);
        b.attack("Target_B");
        b.beRepaired(10);   
    }

    std::cout << "\n========== ClapTrap Energy Exhaustion ==========" << std::endl;
    {
        ClapTrap e("Eve");
        for (int i = 0; i < 10; ++i) {
            e.attack("Dummy_Target");
        }
        e.attack("Target_C");
        e.beRepaired(5);    
        e.takeDamage(5);   
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

    std::cout << "\n========== End of All Tests ==========" << std::endl;
    return 0;
}
