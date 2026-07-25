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
        
        delete instance;
        std::cout << std::endl;
    }

    return 0;
}
