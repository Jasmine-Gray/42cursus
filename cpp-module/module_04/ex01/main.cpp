#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"

int main()
{
    std::cout << "===== Subject Required Test =====" << std::endl;
    const Animal* j = new Dog();
    const Animal* i = new Cat();
    
    std::cout << std::endl;
    delete j; // should not create a leak [cite: 187]
    delete i;

    std::cout << "\n===== Array Test =====" << std::endl;
    const int arraySize = 4;
    Animal* animals[arraySize]; // Animalオブジェクトの配列 [cite: 169]

    for (int k = 0; k < arraySize / 2; k++) {
        animals[k] = new Dog(); // 半分はDog [cite: 170]
    }
    for (int k = arraySize / 2; k < arraySize; k++) {
        animals[k] = new Cat(); // もう半分はCat [cite: 170]
    }

    std::cout << std::endl;
    for (int k = 0; k < arraySize; k++) {
        animals[k]->makeSound();
    }

    std::cout << std::endl;
    for (int k = 0; k < arraySize; k++) {
        delete animals[k]; // ループで全て直接Animalとしてdeleteする [cite: 171, 172]
    }

    std::cout << "\n===== Deep Copy Test =====" << std::endl;
    Dog basicDog;
    {
        Dog tmpDog = basicDog; // コピーコンストラクタの呼び出し
        // tmpDogがスコープを抜けて破棄されても、basicDogのBrainは無事である必要がある 
    }
    std::cout << "If shallow copy occurred, basicDog's brain would be deleted by now, causing double free error on exit." << std::endl;

    std::cout << "\n===== Assignment Deep Copy Test =====" << std::endl;
    Dog dogA;
    Dog dogB;
    dogA.getBrain()->setIdea(0, "I want bones!");
    
    dogB = dogA; // 代入演算子の呼び出し
    dogB.getBrain()->setIdea(0, "I want to sleep!");
    
    std::cout << "Dog A idea: " << dogA.getBrain()->getIdea(0) << std::endl;
    std::cout << "Dog B idea: " << dogB.getBrain()->getIdea(0) << std::endl;
    // それぞれが独立したBrainを持っているため、出力が異なるはずです 。

    return 0; // プログラム終了時に自動的にValgrindなどでリークチェックが行われることを想定 
}
