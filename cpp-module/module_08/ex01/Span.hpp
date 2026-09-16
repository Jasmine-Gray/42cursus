#ifndef SPAN_HPP
#define SPAN_HPP

#include <vector>
#include <exception>
#include <iterator>
#include <algorithm>

class Span {
private:
    unsigned int maxSize;
    std::vector<int> numbers;

public:
    Span();
    Span(unsigned int n);
    Span(const Span& other);
    Span& operator=(const Span& other);
    ~Span();

    void addNumber(int number);
    unsigned int shortestSpan() const;
    unsigned int longestSpan() const;

    template <typename Iterator>
    void addNumbers(Iterator begin, Iterator end) {
        if (std::distance(begin, end) + numbers.size() > maxSize) {
            throw FullException();
        }
        numbers.insert(numbers.end(), begin, end);
    }

    class FullException : public std::exception {
    public:
        virtual const char* what() const throw();
    };

    class NotEnoughElementsException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
};

#endif
