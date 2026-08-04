#include <iostream>
#include <string>
#include "iter.hpp"

// 要素を出力するテンプレート関数 (非const参照)
template <typename T>
void printElement(T& elem) {
    std::cout << elem << " ";
}

// 要素を出力するテンプレート関数 (const参照)
template <typename T>
void printConstElement(const T& elem) {
    std::cout << elem << " ";
}

// 要素の値を変更する通常の関数
void increment(int& n) {
    n++;
}

int main(void) {
    // テスト用の配列
    int intArray[] = {1, 2, 3, 4, 5};
    const int constIntArray[] = {10, 20, 30, 40, 50};
    std::string strArray[] = {"Hello", "42", "World"};

    // 1. 非const配列に対して、テンプレート関数(非const参照)を適用
    std::cout << "--- intArray (original) ---" << std::endl;
    ::iter(intArray, 5, printElement<int>);
    std::cout << std::endl << std::endl;

    // 2. 非const配列の要素を変更する関数を適用
    std::cout << "--- intArray (after increment) ---" << std::endl;
    ::iter(intArray, 5, increment);
    ::iter(intArray, 5, printConstElement<int>); // const参照で出力
    std::cout << std::endl << std::endl;

    // 3. const配列に対して、テンプレート関数(const参照)を適用
    std::cout << "--- constIntArray ---" << std::endl;
    // ::iter(constIntArray, 5, increment); // コンパイルエラーになるのが正しい挙動
    ::iter(constIntArray, 5, printConstElement<int>);
    std::cout << std::endl << std::endl;

    // 4. std::string の配列に対して適用
    std::cout << "--- strArray ---" << std::endl;
    ::iter(strArray, 3, printConstElement<std::string>);
    std::cout << std::endl;

    return 0;
}
