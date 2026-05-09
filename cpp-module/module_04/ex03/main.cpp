#include "Ice.hpp"
#include "Cure.hpp"
#include "Character.hpp"
#include "MateriaSource.hpp"

int main()
{
    std::cout << "===== Subject Required Test =====" << std::endl;
    IMateriaSource* src = new MateriaSource();
    src->learnMateria(new Ice());
    src->learnMateria(new Cure());
    
    ICharacter* me = new Character("me");
    AMateria* tmp;
    
    tmp = src->createMateria("ice");
    me->equip(tmp);
    tmp = src->createMateria("cure");
    me->equip(tmp);
    
    ICharacter* bob = new Character("bob");
    
    me->use(0, *bob);
    me->use(1, *bob);
    
    std::cout << "\n===== Unequip & Memory Leak Prevention Test =====" << std::endl;
    // unequip は delete しないため、事前にアドレスを保持しておく必要があります
    AMateria* droppedMateria = src->createMateria("ice");
    me->equip(droppedMateria);
    me->use(2, *bob);
    
    me->unequip(2);
    // droppedMateria は me から外れたので、自分で delete してリークを防ぐ
    delete droppedMateria;

    std::cout << "\n===== Deep Copy Test =====" << std::endl;
    Character* cloneMe = new Character(*(Character*)me);
    std::cout << "Clone Name: " << cloneMe->getName() << std::endl;
    cloneMe->use(0, *bob); // meが持っていたiceをコピーしているはず
    cloneMe->use(1, *bob); // meが持っていたcureをコピーしているはず

    std::cout << "\n===== Cleanup =====" << std::endl;
    delete bob;
    delete me;
    delete cloneMe;
    delete src;

    return 0;
}
