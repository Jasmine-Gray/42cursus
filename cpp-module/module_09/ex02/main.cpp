#include "PmergeMe.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Error" << std::endl;
        return 1;
    }

    PmergeMe pmergeme;
    if (!pmergeme.parseInput(argc, argv)) {
        std::cerr << "Error" << std::endl;
        return 1;
    }

    pmergeme.execute();
    return 0;
}

