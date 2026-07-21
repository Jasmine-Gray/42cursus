#include <iostream>
#include <cstdlib>
#include <ctime>
#include <exception>
#include "Base.hpp"
#include "A.hpp"
#include "B.hpp"
#include "C.hpp"

Base* generate(void) {
    int random = std::rand() % 3;
    Base* instance = NULL;

    // フィードバックの遵守: switch文を使用。
    // 今回は各ケースで独立した処理を行うため break を使用しますが、
    // もし意図的に次のケースへ流す場合は必ず // fallthrough を記述します。
    switch (random) {
        case 0:
            instance = new A();
            break;
        case 1:
            instance = new B();
            break;
        case 2:
            instance = new C();
            break;
    }
    return instance;
}

void identify(Base* p) {
    // ポインタに対する dynamic_cast は失敗時に NULL を返すことを利用する
    if (dynamic_cast<A*>(p)) {
        std::cout << "Pointer identify: A" << std::endl;
    } else if (dynamic_cast<B*>(p)) {
        std::cout << "Pointer identify: B" << std::endl;
    } else if (dynamic_cast<C*>(p)) {
        std::cout << "Pointer identify: C" << std::endl;
    } else {
        std::cout << "Pointer identify: Unknown" << std::endl;
    }
}

void identify(Base& p) {
    // 参照に対する dynamic_cast は失敗時に例外を投げることを利用する
    // <typeinfo>ヘッダーが禁止されているため、直接std::bad_castではなく
    // 基底クラスであるstd::exceptionでキャッチする
    try {
        (void)dynamic_cast<A&>(p);
        std::cout << "Reference identify: A" << std::endl;
        return;
    } catch (const std::exception& e) {}

    try {
        (void)dynamic_cast<B&>(p);
        std::cout << "Reference identify: B" << std::endl;
        return;
    } catch (const std::exception& e) {}

    try {
        (void)dynamic_cast<C&>(p);
        std::cout << "Reference identify: C" << std::endl;
        return;
    } catch (const std::exception& e) {}

    std::cout << "Reference identify: Unknown" << std::endl;
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    for (int i = 0; i < 5; ++i) {
        std::cout << "--- Test " << i + 1 << " ---" << std::endl;
        Base* instance = generate();
        
        identify(instance);
        identify(*instance);
        
        // フィードバックの遵守: RAIIの概念について
        // C++98ではスマートポインタ(std::unique_ptr等)が標準にないため、
        // 動的確保したメモリはここで明示的に delete してリソースリークを防ぎます。
        delete instance;
        std::cout << std::endl;
    }

    return 0;
}
