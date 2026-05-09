#include "Dog.hpp"

Dog::Dog() {
    this->type = "Dog";
    this->brain = new Brain(); // コンストラクタでBrainをnew [cite: 167]
    std::cout << "Dog default constructor called" << std::endl;
}

// コピーコンストラクタでディープコピーを行う
Dog::Dog(const Dog& other) : AAnimal(other) {
    std::cout << "Dog copy constructor called" << std::endl;
    this->brain = new Brain(*other.brain); // otherのBrainを元に新しく確保 
}

// 代入演算子でディープコピーを行う
Dog& Dog::operator=(const Dog& other) {
    std::cout << "Dog copy assignment operator called" << std::endl;
    if (this != &other) {
        this->type = other.type;
        // 既存のbrainメモリを解放してから新しくコピー 
        delete this->brain;
        this->brain = new Brain(*other.brain);
    }
    return *this;
}

Dog::~Dog() {
    delete this->brain; // デストラクタでBrainをdelete [cite: 168]
    std::cout << "Dog destructor called" << std::endl;
}

void Dog::makeSound() const {
    std::cout << "Woof! Woof!" << std::endl;
}

Brain* Dog::getBrain() const {
    return this->brain;
}
