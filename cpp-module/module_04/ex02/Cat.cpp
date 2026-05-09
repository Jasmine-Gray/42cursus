#include "Cat.hpp"

Cat::Cat() {
    this->type = "Cat";
    this->brain = new Brain(); // コンストラクタでBrainを生成
    std::cout << "Cat default constructor called" << std::endl;
}

// コピーコンストラクタでディープコピーを行う
Cat::Cat(const Cat& other) : AAnimal(other) {
    std::cout << "Cat copy constructor called" << std::endl;
    this->brain = new Brain(*other.brain); // otherのBrainを元に新しくメモリ確保
}

// 代入演算子でディープコピーを行う
Cat& Cat::operator=(const Cat& other) {
    std::cout << "Cat copy assignment operator called" << std::endl;
    if (this != &other) {
        this->type = other.type;
        // 既存のbrainメモリを解放してから新しくコピーする（メモリリーク防止）
        delete this->brain;
        this->brain = new Brain(*other.brain);
    }
    return *this;
}

Cat::~Cat() {
    delete this->brain; // デストラクタでBrainを破棄（RAIIの原則）
    std::cout << "Cat destructor called" << std::endl;
}

void Cat::makeSound() const {
    std::cout << "Meow!" << std::endl;
}

Brain* Cat::getBrain() const {
    return this->brain;
}
