#ifndef CAT_HPP
#define CAT_HPP

#include "AAnimal.hpp"
#include "Brain.hpp" // Brainを使用するために追加

class Cat : public AAnimal {
private:
    Brain* brain; // プライベートなBrainポインタを追加

public:
    Cat();
    Cat(const Cat& other);
    Cat& operator=(const Cat& other);
    ~Cat();

    void makeSound() const;
    Brain* getBrain() const; // ディープコピーのテスト用に追加
};

#endif
