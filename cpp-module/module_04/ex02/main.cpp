#include "AAnimal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"

int main()
{
    std::cout << "===== Abstract Class Test =====" << std::endl;
    
    // 以下の行のコメントアウトを外すと、コンパイルエラーになるはずです！
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

    // ex01のArrayテスト等の動作も今まで通り動作します
    // (AAnimalのポインタ配列として扱うことが可能)

    return 0;
}
