#pragma once
#include <cstdint>

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

        ConstIterator(const Array& array, IteratorDirection direction);

        void next();

        bool hasNext() const;

        const T& get() const;
    };

    class Iterator : public ConstIterator {
    public:
        Iterator(const Array& array) : Iterator(array, FORWARD) {}
        Iterator(const Array& array, IteratorDirection direction) : ConstIterator(array, direction) {}

        void set(const T& value);
    };

    void grow();

public:
    Array() : Array(kDefaultCapacity) {}

    Array(array_size capacity);

    array_size size() {
        return size_;
    }

    array_size capacity() {
        return capacity_;
    }

    int insert(const T& value);
    int insert(array_size index, const T& value);
    void remove(array_size index);


    Array<T>& operator =(const Array<T>& other);
    Array<T>& operator =(Array<T>&& other) noexcept;

    const T& operator[](array_size index) const;
    T& operator[](array_size index);

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

    ~Array();
};