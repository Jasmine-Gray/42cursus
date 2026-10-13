#include "RPN.hpp"
#include <stdexcept>

RPN::RPN() {}

RPN::RPN(const RPN& other) : operandStack(other.operandStack) {}

RPN& RPN::operator=(const RPN& other) {
    if (this != &other) {
        operandStack = other.operandStack;
    }
    return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(const std::string& token) const {
    return token == "+" || token == "-" || token == "*" || token == "/";
}

void RPN::performOperation(const std::string& op) {
    // 演算子に対してスタックに2つ以上の値がない場合はエラー
    if (operandStack.size() < 2) {
        throw std::runtime_error("Error"); 
    }

    // スタックはLIFOなので、先にpopしたものが「右辺(v2)」、次が「左辺(v1)」
    int v2 = operandStack.top();
    operandStack.pop();
    int v1 = operandStack.top();
    operandStack.pop();

    int result = 0;
    
    // 1文字の演算子としてswitch文で判定（すべてbreakするためfallthroughは不要）
    switch (op[0]) {
        case '+':
            result = v1 + v2;
            break;
        case '-':
            result = v1 - v2;
            break;
        case '*':
            result = v1 * v2;
            break;
        case '/':
            if (v2 == 0) {
                throw std::runtime_error("Error"); // ゼロ除算エラー
            }
            result = v1 / v2;
            break;
        default:
            throw std::runtime_error("Error");
    }
    
    // 計算結果をスタックに戻す
    operandStack.push(result);
}

void RPN::calculate(const std::string& expression) {
    std::istringstream iss(expression); // RAIIにより自動で破棄される
    std::string token;

    // スペース区切りでトークンを読み込む
    while (iss >> token) {
        if (isOperator(token)) {
            try {
                performOperation(token);
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
                return;
            }
        } else {
            // トークンが10未満の数値(0〜9)であるかの検証
            if (token.length() != 1 || !std::isdigit(token[0])) {
                std::cerr << "Error" << std::endl;
                return;
            }
            
            // 数値であればスタックに積む ('0'を引いてintに変換)
            int num = token[0] - '0';
            operandStack.push(num);
        }
    }

    // EOF以外の致命的なストリームエラーが発生したか状態ビットで確認
    if (iss.bad()) {
        std::cerr << "Error" << std::endl;
        return;
    }

    // 最終的にスタックに残る値は1つ（最終結果）でなければならない
    if (operandStack.size() != 1) {
        std::cerr << "Error" << std::endl;
        return;
    }

    // 正しい結果を標準出力に表示
    std::cout << operandStack.top() << std::endl;
}


