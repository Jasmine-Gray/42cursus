#ifndef ARRAY_TPP
# define ARRAY_TPP

template <typename T>
Array<T>::Array() : elements(NULL), length(0) {}

template <typename T>
Array<T>::Array(unsigned int n) : elements(new T[n]()), length(n) {}

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

template <typename T>
Array<T>& Array<T>::operator=(const Array& other) {
    if (this != &other) {
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

template <typename T>
Array<T>::~Array() {
    delete[] this->elements;
}

template <typename T>
T& Array<T>::operator[](unsigned int index) {
    if (index >= this->length) {
        throw OutOfBoundsException();
    }
    return this->elements[index];
}

template <typename T>
const T& Array<T>::operator[](unsigned int index) const {
    if (index >= this->length) {
        throw OutOfBoundsException();
    }
    return this->elements[index];
}

template <typename T>
unsigned int Array<T>::size() const {
    return this->length;
}

template <typename T>
const char* Array<T>::OutOfBoundsException::what() const throw() {
    return "Error: Index is out of bounds.";
}

#endif 
