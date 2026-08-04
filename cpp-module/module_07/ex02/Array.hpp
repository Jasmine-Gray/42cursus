#ifndef ARRAY_HPP
# define ARRAY_HPP

# include <exception>
# include <cstddef>

template <typename T>
class Array {
private:
    T*              elements;
    unsigned int    length;

public:
    // Orthodox Canonical Form
    Array();
    Array(unsigned int n);
    Array(const Array& other);
    Array& operator=(const Array& other);
    ~Array();

    // 添字演算子のオーバーロード (非const と const)
    T& operator[](unsigned int index);
    const T& operator[](unsigned int index) const;

    // 要素数を返す関数
    unsigned int size() const;

    // 範囲外アクセスのための例外クラス
    class OutOfBoundsException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
};

// テンプレートの実装をインクルード
# include "Array.tpp"

#endif // ARRAY_HPP
