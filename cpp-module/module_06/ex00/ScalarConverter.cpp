#include "ScalarConverter.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <cmath>
#include <cctype>

ScalarConverter::ScalarConverter() {}

ScalarConverter::ScalarConverter(const ScalarConverter& src) {
    (void)src;
}

ScalarConverter& ScalarConverter::operator=(const ScalarConverter& rhs) {
    (void)rhs;
    return *this;
}

ScalarConverter::~ScalarConverter() {}

void ScalarConverter::convert(const std::string& literal) {
    // 1. 疑似リテラル（pseudo-literals）の処理
    if (literal == "nan" || literal == "nanf") {
        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        std::cout << "float: nanf\n";
        std::cout << "double: nan\n";
        return;
    }
    if (literal == "+inf" || literal == "+inff") {
        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        std::cout << "float: +inff\n";
        std::cout << "double: +inf\n";
        return;
    }
    if (literal == "-inf" || literal == "-inff") {
        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        std::cout << "float: -inff\n";
        std::cout << "double: -inf\n";
        return;
    }

    double val = 0.0;
    
    // 2. 単一文字（char）リテラルの検出 ('0'~'9'の数字単体は除外)
    if (literal.length() == 1 && !std::isdigit(literal[0])) {
        val = static_cast<double>(literal[0]);
    } else {
        // 3. 数値としてのパース処理
        std::string s = literal;
        
        // floatリテラル末尾の 'f' を除去してパース可能にする
        if (s.length() > 1 && s[s.length() - 1] == 'f') {
            s.erase(s.length() - 1);
        }

        // [フィードバック反映]: stringstream と状態ビットによる厳格なエラーハンドリング
        // RAIIに基づき、ssはスコープ終了時に自動で破棄・解放されます。
        std::stringstream ss(s);
        ss >> val;

        // failbit（変換失敗）または eofbitが立っていない（ゴミ文字が残っている）場合を検知
        if (ss.fail() || !ss.eof()) {
            std::cout << "char: impossible\n"
                      << "int: impossible\n"
                      << "float: impossible\n"
                      << "double: impossible\n";
            return;
        }
    }

    // 4. 各スカラー型への明示的なキャストと出力

    // --- char型 ---
    if (val < std::numeric_limits<char>::min() || val > std::numeric_limits<char>::max() || std::isnan(val) || std::isinf(val)) {
        std::cout << "char: impossible\n";
    } else {
        char c = static_cast<char>(val);
        if (std::isprint(c)) {
            std::cout << "char: '" << c << "'\n";
        } else {
            std::cout << "char: Non displayable\n";
        }
    }

    // --- int型 ---
    // C++98ではオーバーフローしたdoubleからintへのキャストは未定義動作になるため、先に範囲チェックを行う
    if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max() || std::isnan(val) || std::isinf(val)) {
        std::cout << "int: impossible\n";
    } else {
        std::cout << "int: " << static_cast<int>(val) << "\n";
    }

    // --- float型 ---
    float fval = static_cast<float>(val);
    std::cout << "float: " << fval;
    // 整数値の場合は ".0" を補完する（小数点以下の情報が欠落しないように）
    if (val == std::floor(val) && !std::isinf(val) && !std::isnan(val)) {
        std::cout << ".0";
    }
    std::cout << "f\n";

    // --- double型 ---
    std::cout << "double: " << val;
    if (val == std::floor(val) && !std::isinf(val) && !std::isnan(val)) {
        std::cout << ".0";
    }
    std::cout << "\n";
}
