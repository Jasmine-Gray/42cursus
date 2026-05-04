#include "ClapTrap.hpp"
#include <iostream>

int main() {
    std::cout << "========== [1] Constructor & OCF Tests ==========" << std::endl;
    ClapTrap a;                     // Default constructor
    ClapTrap b("Bob");              // Parameterized constructor
    ClapTrap c(b);                  // Copy constructor
    ClapTrap d("Dave");
    d = b;                          // Copy assignment operator

    std::cout << "\n========== [2] Basic Action Tests ==========" << std::endl;
    // 初期状態: HP=10, EP=10, AD=0
    b.attack("Target_A");           // EP: 10 -> 9
    b.takeDamage(5);                // HP: 10 -> 5
    b.beRepaired(3);                // EP: 9 -> 8, HP: 5 -> 8

    std::cout << "\n========== [3] Corner Case: Overkill & HP 0 ==========" << std::endl;
    // HPアンダーフロー保護の確認
    b.takeDamage(100);              // HP: 8 -> 0 (マイナスにならない)
    
    // HPが0になったので、以下の行動は失敗するはず
    b.attack("Target_B");
    b.beRepaired(10);

    std::cout << "\n========== [4] Corner Case: Energy Exhaustion ==========" << std::endl;
    ClapTrap e("Eve");              // 初期状態: HP=10, EP=10

    // EPを0まで使い切る
    for (int i = 0; i < 10; ++i) {
        e.attack("Dummy_Target");
    }
    
    // EPが0になったので、以下の行動は失敗するはず
    e.attack("Target_C");
    e.beRepaired(5);

    // ダメージを受けることはEPに関係なく可能
    e.takeDamage(5);

    std::cout << "\n========== [5] Destructor Tests (RAII in action) ==========" << std::endl;
    // 戻り値0で終了し、スコープを抜ける際にスタック上の変数が逆順で破棄される
    // (デストラクタが e -> d -> c -> b -> a の順で呼ばれる)
    return 0;
}
