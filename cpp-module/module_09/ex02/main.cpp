#define RUN_TESTS

#include "PmergeMe.hpp"
#include <iostream>
#include <vector>
#include <string>

#ifdef RUN_TESTS

void runSingleTest(const std::string& testName, const std::vector<std::string>& inputArgs) {
    std::cout << "========================================" << std::endl;
    std::cout << "Test: " << testName << std::endl;
    std::cout << "Input: ";
    for (size_t i = 0; i < inputArgs.size(); ++i) {
        std::cout << inputArgs[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // argv 配列と文字列のバッファを安全に作成 (ポインタ破棄を防止)
    std::vector<std::string> tempStorage;
    tempStorage.push_back("./PmergeMe");
    for (size_t i = 0; i < inputArgs.size(); ++i) {
        tempStorage.push_back(inputArgs[i]);
    }

    std::vector<char*> argvPointers;
    for (size_t i = 0; i < tempStorage.size(); ++i) {
        argvPointers.push_back(&tempStorage[i][0]);
    }

    PmergeMe pmergeme;
    if (!pmergeme.parseInput(static_cast<int>(argvPointers.size()), &argvPointers[0])) {
        std::cerr << "Error" << std::endl;
    } else {
        pmergeme.execute();
    }
    std::cout << std::endl;
}

int main() {
    std::cout << ">>> RUNNING MANUAL TEST SUITE <<<\n" << std::endl;

    // 1. 基本的な正常系テスト (奇数個)[cite: 30]
    {
        std::vector<std::string> args;
        args.push_back("3"); args.push_back("5"); args.push_back("9"); args.push_back("7"); args.push_back("4");
        runSingleTest("Basic Odd Sequence", args);
    }

    // 2. 正常系テスト (偶数個)
    {
        std::vector<std::string> args;
        args.push_back("7"); args.push_back("5"); args.push_back("3"); args.push_back("2"); args.push_back("45"); args.push_back("1");
        runSingleTest("Basic Even Sequence", args);
    }

    // 3. 重複値を含むテスト (Subject要件: "duplicate values included")[cite: 29]
    {
        std::vector<std::string> args;
        args.push_back("4"); args.push_back("6"); args.push_back("3"); args.push_back("2"); args.push_back("6"); args.push_back("4");
        runSingleTest("Duplicate Values", args);
    }

    // 4. すでにソートされている入力テスト
    {
        std::vector<std::string> args;
        args.push_back("1"); args.push_back("2"); args.push_back("3"); args.push_back("4"); args.push_back("5");
        runSingleTest("Already Sorted", args);
    }

    // 5. 逆順の入力テスト
    {
        std::vector<std::string> args;
        args.push_back("5"); args.push_back("4"); args.push_back("3"); args.push_back("2"); args.push_back("1");
        runSingleTest("Reverse Order", args);
    }

    // 6. 要素数が1つの限界値テスト
    {
        std::vector<std::string> args;
        args.push_back("42");
        runSingleTest("Single Element", args);
    }

    // 7. 異常系: 負の数[cite: 30]
    {
        std::vector<std::string> args;
        args.push_back("-1"); args.push_back("2");
        runSingleTest("Error Test (Negative Number)", args);
    }

    // 8. 異常系: 文字列混入
    {
        std::vector<std::string> args;
        args.push_back("1"); args.push_back("2"); args.push_back("abc"); args.push_back("4");
        runSingleTest("Error Test (Non-numeric)", args);
    }

    // 9. 異常系: int型最大値オーバーフロー[cite: 26]
    {
        std::vector<std::string> args;
        args.push_back("2147483648");
        runSingleTest("Error Test (Overflow > INT_MAX)", args);
    }

    return 0;
}

#else

// 通常実行 (コマンドライン引数から受け取る)[cite: 30]
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

#endif
