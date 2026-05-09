#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

int main()
{
    std::cout << "===== Subject Tests =====" << std::endl;
    const Animal* meta = new Animal();
    const Animal* j = new Dog();
    const Animal* i = new Cat();
    
    std::cout << std::endl;
    std::cout << j->getType() << " " << std::endl;
    std::cout << i->getType() << " " << std::endl;
    
    i->makeSound(); // will output the cat sound!
    j->makeSound(); // will output the dog sound!
    meta->makeSound(); // will output the animal sound!
    
    std::cout << std::endl;
    delete i;
    delete j;
    delete meta;

    std::cout << "\n===== WrongAnimal Tests =====" << std::endl;
    const WrongAnimal* wrongMeta = new WrongAnimal();
    const WrongAnimal* wrongI = new WrongCat();
    
    std::cout << std::endl;
    std::cout << wrongI->getType() << " " << std::endl;
    
    // ここは仮想関数ではないため、WrongCatではなくWrongAnimalの音が鳴る
    wrongI->makeSound(); 
    wrongMeta->makeSound();
    
    std::cout << std::endl;
    delete wrongI;
    delete wrongMeta;

    std::cout << "\n===== Extra Tests (Direct instances) =====" << std::endl;
    Dog dog1;
    Cat cat1;
    WrongCat wcat1;

    dog1.makeSound();
    cat1.makeSound();
    wcat1.makeSound(); // ポインタを介さず直接呼べばWrong Meow!と鳴る

    return 0;
}
