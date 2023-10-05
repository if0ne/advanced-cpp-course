#include "Array.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

template<typename T>
Array<T>::ConstIterator::ConstIterator(const Array& array, IteratorDirection direction) : direction_(direction) {
    switch (direction)
    {
    case FORWARD:
        current_ = array.data_;
        last_ = array.data_ + array.size_;
        break;

    case BACKWARD:
        current_ = array.data_ + array.size_ - 1;
        last_ = array.data_;
        break;
    };
}

template<typename T>
void Array<T>::ConstIterator::next() {
    current_ += direction_;
}

template<typename T>
bool Array<T>::ConstIterator::hasNext() const {
    switch (direction_)
    {
    case FORWARD:
        return current_ < last_;

    case BACKWARD:
        return current_ >= last_;

    default:
        return false;
    };
}

template<typename T>
const T& Array<T>::ConstIterator::get() const {
    return *current_;
}

template<typename T>
void Array<T>::Iterator::set(const T& value) {
    this->current_ = value;
}

template<typename T>
void Array<T>::grow() {
    array_size new_capacity = kGrowFactor * capacity_;
    T* new_data = (T*)malloc(new_capacity * sizeof(T));

    assert(new_data);

    memmove(new_data, data_, size_ * sizeof(T));

    free(data_);
    data_ = new_data;
    capacity_ = new_capacity;
}

template<typename T>
Array<T>::Array(array_size capacity) {
    assert(capacity > 0);

    data_ = (T*)malloc(capacity * sizeof(T));

    assert(data_);

    capacity_ = capacity;
    size_ = 0;
}

template<typename T>
int Array<T>::insert(const T&& value) {
    if (size_ == capacity_) {
        grow();
    }

    data_[size_] = value;

    size_++;

    return size_ - 1;
}

template<typename T>
int Array<T>::insert(array_size index, const T& value) {
    assert(index >= 0);
    assert(index < size_);

    if (size_ == capacity_) {
        grow();
    }

    for (array_size i = size_; i > index; --i) {
        data_[i] = data_[i - 1];
    }

    data_[index] = value;
    size_++;

    return size_ - 1;
}

template<typename T>
void Array<T>::remove(array_size index) {
    assert(index >= 0);
    assert(index < size_);

    for (array_size i = index; i < size_; ++i) {
        data_[i] = data_[i + 1];
    }
}

template<typename T>
const T& Array<T>::operator[](array_size index) const {
    assert(index >= 0);
    assert(index < size_);

    return data_[index];
}

template<typename T>
T& Array<T>::operator[](array_size index) {
    assert(index >= 0);
    assert(index < size_);

    return data_[index];
}

template<typename T>
Array<T>& Array<T>::operator=(const Array<T>& other)
{
    if (this == &other) return *this;
    ~Array();

    data_ = (T*)malloc(other.capacity_ * sizeof(T));

    assert(data_);

    memcpy(data_, other.data_, other.size_ * sizeof(T));

    size_ = other.size_;
    capacity_ = other.capacity_;

    return *this;
}

template<typename T>
Array<T>& Array<T>::operator=(Array<T>&& other) noexcept
{
    if (this == &other) return *this;
    ~Array();

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;

    return *this;
}

template<typename T>
Array<T>::~Array() {
    for (array_size i = 0; i < size_; ++i) {
        data_[i].~T();
    }

    free(data_);
}