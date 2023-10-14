#pragma once
#include <cstdint>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <utility>

#if INTPTR_MAX == INT32_MAX
typedef int32_t array_size;
#elif INTPTR_MAX == INT64_MAX
typedef int64_t array_size;
#else
#error "Environment not 32 or 64-bit."
#endif

template<typename T>
class Array final {
private:
    static const array_size kDefaultCapacity = 8;
    static const array_size kGrowFactor = 2;

    T* data_;
    array_size size_;
    array_size capacity_;

    enum IteratorDirection {
        FORWARD = 1,
        BACKWARD = -1
    };

    class ConstIterator {
    protected:
        T* current_;
        const T* last_;

        IteratorDirection direction_;

    public:
        ConstIterator(const Array& array) : ConstIterator(array, FORWARD) {}

        ConstIterator(const Array& array, IteratorDirection direction) : direction_(direction) {
            switch (direction)
            {
            case FORWARD:
                current_ = array.data_;
                last_ = array.data_ + array.size_ - 1;
                break;

            case BACKWARD:
                current_ = array.data_ + array.size_ - 1;
                last_ = array.data_;
                break;
            };
        }

        void next() {
            current_ += direction_;
        }

        bool hasNext() const {
            switch (direction_)
            {
            case FORWARD:
                return current_ < last_;

            case BACKWARD:
                return current_ > last_;

            default:
                return false;
            }
        };

        const T& get() const {
            return *current_;
        }

        const T& operator*() const {
            return *current_;
        }

        const ConstIterator& operator++() const {
            next();
            return *this;
        }

        ConstIterator operator++(int) const {
            ConstIterator temp = *this;
            ++*this;
            return temp;
        }
    };

    class Iterator : public ConstIterator {
    public:
        Iterator(const Array& array) : Iterator(array, FORWARD) {}
        Iterator(const Array& array, IteratorDirection direction) : ConstIterator(array, direction) {}

        void set(const T& value) {
            this->current_ = value;
        }

        Iterator& operator++() {
            this->next();
            return *this;
        }

        T& operator*() {
            return *this->current_;
        }
    };

    void grow() {
        array_size new_capacity = kGrowFactor * capacity_;
        T* new_data = (T*)malloc(new_capacity * sizeof(T));

        assert(new_data);

        for (array_size i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }

        free(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    }

    void swap(Array<T>& lhs, Array<T>& rhs) {
        std::swap(lhs.data_, rhs.data_);
        std::swap(lhs.size_, rhs.size_);
        std::swap(lhs.capacity_, rhs.capacity_);
    }

public:
    Array() : Array(kDefaultCapacity) {}

    Array(array_size capacity) {
        assert(capacity > 0);

        data_ = (T*)malloc(capacity * sizeof(T));

        assert(data_);

        capacity_ = capacity;
        size_ = 0;
    }

    Array(const Array<T>& other) {
        data_ = (T*)malloc(other.capacity_ * sizeof(T));

        assert(data_);

        for (array_size i = 0; i < other.size_; ++i) {
            new (data_ + i) T(other.data_[i]);
        }

        size_ = other.size_;
        capacity_ = other.capacity_;
    }

    Array(Array<T>&& other) {
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
    }


    array_size size() {
        return size_;
    }

    array_size capacity() {
        return capacity_;
    }

    int insert(const T& value) {
        assert(data_);

        if (size_ == capacity_) {
            grow();
        }

        new (data_ + size_) T(value);

        size_++;

        return size_ - 1;
    }

    int insert(T&& value) {
        assert(data_);

        if (size_ == capacity_) {
            grow();
        }

        new (data_ + size_) T(std::move(value));

        size_++;

        return size_ - 1;
    }

    int insert(array_size index, const T& value) {
        assert(data_);
        assert(index >= 0);
        assert(index < size_);

        if (size_ == capacity_) {
            grow();
        }

        for (array_size i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }

        new (data_ + index) T(value);
        size_++;

        return size_ - 1;
    }

    int insert(array_size index, T&& value) {
        assert(data_);
        assert(index >= 0);
        assert(index < size_);

        if (size_ == capacity_) {
            grow();
        }

        for (array_size i = size_; i > index; --i) {
            new (data_ + i) T(std::move(data_[i - 1]));
        }

        new (data_ + index) T(std::move(value));
        size_++;

        return size_ - 1;
    }

    void remove(array_size index) {
        assert(data_);
        assert(index >= 0);
        assert(index < size_);

        data_[index].~T();

        for (array_size i = index; i < size_; ++i) {
            new (data_ + i) T(std::move(data_[i + 1]));
        }

        size_--;
    }

    const T* begin() const {
        return data_;
    }

    const T* end() const {
        return data_ + size_;
    }

    T* begin() {
        return data_;
    }

    T* end() {
        return data_ + size_;
    }

    Array<T>& operator =(const Array<T>& other) {
        swap(*this, other);

        return *this;
    }

    Array<T>& operator =(Array<T>&& other) {
        Array<T> temp{ other };
        swap(*this, temp);

        return *this;
    }

    const T& operator[](array_size index) const {
        assert(index >= 0);
        assert(index < size_);

        return data_[index];
    }

    T& operator[](array_size index) {
        assert(index >= 0);
        assert(index < size_);

        return data_[index];
    }

    Iterator iterator() {
        return Iterator{ *this, FORWARD };
    }

    ConstIterator iterator() const {
        return ConstIterator{ *this, FORWARD };
    }

    Iterator reverseIterator() {
        return Iterator{ *this, BACKWARD };
    }

    ConstIterator reverseIterator() const {
        return ConstIterator{ *this, BACKWARD };
    }

    ~Array() {
        if (!data_) {
            return;
        }

        for (array_size i = 0; i < size_; ++i) {
            data_[i].~T();
        }

        free(data_);
    }
};

