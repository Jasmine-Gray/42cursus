#include "AAnimal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"

int main()
{
    std::cout << "===== Abstract Class Test =====" << std::endl;
    
    // 以下の行のコメントアウトを外すと、コンパイルエラー
    // AAnimal* cannotInstantiate = new AAnimal();
    // AAnimal cannotInstantiate2;

    const AAnimal* j = new Dog();
    const AAnimal* i = new Cat();
    
    std::cout << std::endl;
    std::cout << j->getType() << " " << std::endl;
    std::cout << i->getType() << " " << std::endl;
    j->makeSound();
    i->makeSound();
    
    std::cout << std::endl;
    delete j;
    delete i;

    return 0;
}
