#include "Character.hpp"
#include "AMateria.hpp"

Character::Character() : name("Default") {
    for (int i = 0; i < 4; i++) {
        inventory[i] = NULL;
    }
}

Character::Character(std::string const & name) : name(name) {
    for (int i = 0; i < 4; i++) {
        inventory[i] = NULL;
    }
}

// 深いコピーを行うコピーコンストラクタ
Character::Character(const Character& other) : name(other.name) {
    for (int i = 0; i < 4; i++) {
        if (other.inventory[i])
            this->inventory[i] = other.inventory[i]->clone();
        else
            this->inventory[i] = NULL;
    }
}

// 深いコピーを行う代入演算子
Character& Character::operator=(const Character& other) {
    if (this != &other) {
        this->name = other.name;
        for (int i = 0; i < 4; i++) {
            // Subjectの指示通り、既存のマテリアを削除してからコピーする
            if (this->inventory[i]) {
                delete this->inventory[i];
            }
            if (other.inventory[i]) {
                this->inventory[i] = other.inventory[i]->clone();
            } else {
                this->inventory[i] = NULL;
            }
        }
    }
    return *this;
}

Character::~Character() {
    for (int i = 0; i < 4; i++) {
        if (inventory[i]) {
            delete inventory[i];
        }
    }
}

std::string const & Character::getName() const {
    return this->name;
}

void Character::equip(AMateria* m) {
    if (!m) return;
    for (int i = 0; i < 4; i++) {
        if (inventory[i] == NULL) {
            inventory[i] = m;
            return;
        }
    }
}

void Character::unequip(int idx) {
    // Subjectの指示: "The unequip() member function must NOT delete the Materia!"
    if (idx >= 0 && idx < 4) {
        inventory[idx] = NULL;
    }
}

void Character::use(int idx, ICharacter& target) {
    if (idx >= 0 && idx < 4 && inventory[idx]) {
        inventory[idx]->use(target);
    }
}
