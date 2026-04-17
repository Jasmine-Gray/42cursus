#include "Zombie.hpp"

Zombie* newZombie(std::string name);
void randomChump(std::string name);

int main(void) {
    std::cout << "----- Testing randomChump (Stack) -----" << std::endl;
    randomChump("StackZombie");

    std::cout << std::endl;

    std::cout << "----- Testing newZombie (Heap) -----" << std::endl;
    Zombie* heapZombie = newZombie("HeapZombie");
    
    heapZombie->announce();

    std::cout << "Deleting heapZombie..." << std::endl;
    delete heapZombie;

    std::cout << std::endl;
    std::cout << "----- End of tests -----" << std::endl;

    return (0);
}
