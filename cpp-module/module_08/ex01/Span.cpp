#include "Span.hpp"
#include <algorithm>
#include <limits>

// Orthodox Canonical Form -----------------------------------------------------

Span::Span() : maxSize(0) {}

Span::Span(unsigned int n) : maxSize(n) {}

Span::Span(const Span& other) : maxSize(other.maxSize), numbers(other.numbers) {}

Span& Span::operator=(const Span& other) {
    if (this != &other) {
        maxSize = other.maxSize;
        numbers = other.numbers;
    }
    return *this;
}

Span::~Span() {}

// Member Functions ------------------------------------------------------------

void Span::addNumber(int number) {
    if (numbers.size() >= maxSize) {
        throw FullException();
    }
    numbers.push_back(number);
}

unsigned int Span::shortestSpan() const {
    if (numbers.size() <= 1) {
        throw NotEnoughElementsException();
    }

    // コンテナのコピーを作成してソートする（const関数内で元データを変更しないため）
    std::vector<int> sortedNumbers = numbers;
    std::sort(sortedNumbers.begin(), sortedNumbers.end());

    unsigned int minSpan = std::numeric_limits<unsigned int>::max();
    for (size_t i = 1; i < sortedNumbers.size(); ++i) {
        unsigned int span = static_cast<unsigned int>(sortedNumbers[i] - sortedNumbers[i - 1]);
        if (span < minSpan) {
            minSpan = span;
        }
    }
    return minSpan;
}

unsigned int Span::longestSpan() const {
    if (numbers.size() <= 1) {
        throw NotEnoughElementsException();
    }

    // std::min_element と std::max_element を使用して最大値と最小値を求める
    std::vector<int>::const_iterator minIt = std::min_element(numbers.begin(), numbers.end());
    std::vector<int>::const_iterator maxIt = std::max_element(numbers.begin(), numbers.end());
    
    return static_cast<unsigned int>(*maxIt - *minIt);
}

// Exception Definitions -------------------------------------------------------

const char* Span::FullException::what() const throw() {
    return "Error: Span is already full.";
}

const char* Span::NotEnoughElementsException::what() const throw() {
    return "Error: Not enough elements to find a span.";
}

