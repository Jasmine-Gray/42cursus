#ifndef AANIMAL_HPP
#define AANIMAL_HPP

#include <iostream>
#include <string>

class AAnimal {
protected:
    std::string type;

public:
    AAnimal();
    AAnimal(const AAnimal& other);
    AAnimal& operator=(const AAnimal& other);
    virtual ~AAnimal();

    std::string getType() const;
    
    // 純粋仮想関数 (Pure Virtual Function)
    virtual void makeSound() const = 0; 
};

#endif
