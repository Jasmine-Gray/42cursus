#ifndef ITER_HPP
# define ITER_HPP

# include <cstddef>

template <typename T, typename Func>
void iter(T* array, const std::size_t length, Func func) {
    for (std::size_t i = 0; i < length; ++i) {
        func(array[i]);
    }
}

template <typename T, typename Func>
void iter(const T* array, const std::size_t length, Func func) {
    for (std::size_t i = 0; i < length; ++i) {
        func(array[i]);
    }
}

#endif
