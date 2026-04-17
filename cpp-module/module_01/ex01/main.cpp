#include "Zombie.hpp"
#include <iostream>

Zombie* zombieHorde(int N, std::string name);

int main()
{
    int N = 5;
    std::string hordeName = "Walker";

    std::cout << "--- Creating a horde of " << N << " zombies named " << hordeName << " ---" << std::endl;
    
    Zombie* horde = zombieHorde(N, hordeName);

    if (!horde)
    {
        std::cerr << "Failed to allocate memory for the zombie horde." << std::endl;
        return 1;
    }

    std::cout << "\n--- Zombies are announcing themselves ---" << std::endl;
    for (int i = 0; i < N; i++)
    {
        std::cout << "Zombie [" << i << "] : ";
        horde[i].announce();
    }

    std::cout << "\n--- Destroying the horde ---" << std::endl;
    delete[] horde;

    return 0;
}
