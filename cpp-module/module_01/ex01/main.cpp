#include "Zombie.hpp"

int main(void) {
    int N = 5;

    std::cout << "--- Spawning a horde of " << N << " zombies ---" << std::endl;
    
    Zombie* horde = zombieHorde(N, "Minion");

    if (horde == NULL) {
        std::cout << "Failed to create horde." << std::endl;
        return 1;
    }

    for (int i = 0; i < N; i++) {
        horde[i].announce();
    }

    std::cout << "--- Destroying the horde ---" << std::endl;
    delete[] horde;
    
    // std::cout << "--- Test 1: 3 Walkers ---" << std::endl;
    // Zombie* horde1 = zombieHorde(3, "Walker");
    // for (int i = 0; i < 3; i++) horde1[i].announce();
    // delete[] horde1;

    // std::cout << "\n--- Test 2: 7 Clickers ---" << std::endl;
    // Zombie* horde2 = zombieHorde(7, "Clicker");
    // for (int i = 0; i < 7; i++) horde2[i].announce();
    // delete[] horde2;

    // std::cout << "\n--- Test 3: Invalid number (-5) ---" << std::endl;
    // Zombie* horde3 = zombieHorde(-5, "ErrorZom");
    // if (horde3 == NULL) {
    //     std::cout << "Good! Invalid input was blocked." << std::endl;
    // }


    return 0;
}
