#ifndef DIAMONDTRAP_HPP
#define DIAMONDTRAP_HPP

#include "ScavTrap.hpp"
#include "FragTrap.hpp"

class DiamondTrap : public ScavTrap, public FragTrap {
private:
    std::string name; // ClapTrapと同じ変数名にする（シャドウイング）

public:
    // Orthodox Canonical Form
    DiamondTrap();
    DiamondTrap(const std::string& name);
    DiamondTrap(const DiamondTrap& other);
    DiamondTrap& operator=(const DiamondTrap& other);
    ~DiamondTrap();

    // オーバーライドしてScavTrapのものを呼ぶ
    void attack(const std::string& target);

    // 独自の特殊能力
    void whoAmI();
};

#endif
