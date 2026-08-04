#ifndef ARRAY_TPP
# define ARRAY_TPP

// デフォルトコンストラクタ: 空の配列を作成
template <typename T>
Array<T>::Array() : elements(NULL), length(0) {}

// 引数付きコンストラクタ: n個の要素をデフォルト初期化して作成
// Tipの指定通り new T() を使用して初期化を保証する
template <typename T>
Array<T>::Array(unsigned int n) : elements(new T[n]()), length(n) {}

// コピーコンストラクタ: ディープコピーを行う
template <typename T>
Array<T>::Array(const Array& other) : elements(NULL), length(0) {
    if (other.length > 0) {
        this->elements = new T[other.length]();
        this->length = other.length;
        for (unsigned int i = 0; i < this->length; ++i) {
            this->elements[i] = other.elements[i];
        }
    }
}

// 代入演算子: ディープコピーを行う
template <typename T>
Array<T>& Array<T>::operator=(const Array& other) {
    if (this != &other) {
        // 既存のリソースを解放 (RAIIの原則)
        delete[] this->elements;
        
        this->length = other.length;
        if (this->length > 0) {
            this->elements = new T[this->length]();
            for (unsigned int i = 0; i < this->length; ++i) {
                this->elements[i] = other.elements[i];
            }
        } else {
            this->elements = NULL;
        }
    }
    return *this;
}

// デストラクタ: リソースの確実な解放
template <typename T>
Array<T>::~Array() {
    delete[] this->elements;
}

// 添字演算子: 境界チェック付き
template <typename T>
T& Array<T>::operator[](unsigned int index) {
    if (index >= this->length) {
        throw OutOfBoundsException();
    }
    return this->elements[index];
}

// 添字演算子 (const用): 境界チェック付き
template <typename T>
const T& Array<T>::operator[](unsigned int index) const {
    if (index >= this->length) {
        throw OutOfBoundsException();
    }
    return this->elements[index];
}

// size関数
template <typename T>
unsigned int Array<T>::size() const {
    return this->length;
}

// 例外のメッセージ
template <typename T>
const char* Array<T>::OutOfBoundsException::what() const throw() {
    return "Error: Index is out of bounds.";
}

#endif // ARRAY_TPP
