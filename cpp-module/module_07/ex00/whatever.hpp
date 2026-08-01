#ifndef WHATEVER_HPP
# define WHATEVER_HPP

/**
 * @brief Swaps the values of two given parameters.
 * Does not return anything.
 */
template <typename T>
void swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

/**
 * @brief Compares the two values passed as parameters and returns the smallest one.
 * If they are equal, it returns the second one.
 */
template <typename T>
const T& min(const T& a, const T& b) {
    // b が a 以下（等しい場合を含む）であれば2つ目の引数 b を返す
    return (b <= a) ? b : a;
}

/**
 * @brief Compares the two values passed as parameters and returns the greatest one.
 * If they are equal, it returns the second one.
 */
template <typename T>
const T& max(const T& a, const T& b) {
    // b が a 以上（等しい場合を含む）であれば2つ目の引数 b を返す
    return (b >= a) ? b : a;
}

#endif // WHATEVER_HPP
