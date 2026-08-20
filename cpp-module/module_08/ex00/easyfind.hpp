#ifndef EASYFIND_HPP
#define EASYFIND_HPP

#include <algorithm>
#include <exception>

// 要素が見つからなかった場合の例外クラス
class NotFoundException : public std::exception {
public:
    virtual const char* what() const throw() {
        return "Error: Element not found in the container.";
    }
};

// テンプレート関数 easyfind
template <typename T>
typename T::iterator easyfind(T& container, int value) {
    // STLアルゴリズムのstd::findを使用して検索
    typename T::iterator it = std::find(container.begin(), container.end(), value);
    
    // コンテナの終端まで達した（見つからなかった）場合は例外を投げる
    if (it == container.end()) {
        throw NotFoundException();
    }
    
    return it;
}

// const用のオーバーロード（コンテナがconstの場合にも対応）
template <typename T>
typename T::const_iterator easyfind(const T& container, int value) {
    typename T::const_iterator it = std::find(container.begin(), container.end(), value);
    
    if (it == container.end()) {
        throw NotFoundException();
    }
    
    return it;
}

#endif
