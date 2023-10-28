#pragma once
#include <utility>
#include <vector>
#include <cstdlib>
#include <ctime>

std::vector<int> generate(int n) {
    //srand(time(nullptr));
    std::vector<int> foo;

    for (int i = 0; i < n; i++) {
        foo.push_back(n - i);
    }

    return foo;
}

std::vector<int> generate_rand(int n) {
    srand(time(nullptr));
    std::vector<int> foo;

    for (int i = 0; i < n; i++) {
        foo.push_back(rand() % (1 << 24));
    }

    return foo;
}

template<typename T>
void swap(T& left, T& right) {
    T temp(std::move(left));
    left = std::move(right);
    right = std::move(temp);
}

template<typename T, typename Compare>
T* selectPivot(T* first, T* last, Compare comp) {
    T& first_pivot = *first;
    T& last_pivot = *last;
    T& middle_pivot = *(first + (last - first) / 2);

    if (comp(first_pivot, middle_pivot)) {
        swap(first_pivot, middle_pivot);
    }

    if (comp(middle_pivot, last_pivot)) {
        swap(middle_pivot, last_pivot);
    }

    if (comp(first_pivot, middle_pivot)) {
        swap(first_pivot, middle_pivot);
    }

    return &middle_pivot;
}

template<typename T, typename Compare>
T* partition(T* first, T* last, Compare comp) {
    T* pivot = selectPivot(first, last, comp);
    swap(*pivot, *last);
    pivot = last;

    T* i = first - 1;
    T* j = last + 1;

    while (true) {
        do { i++; } while (comp(*i, *pivot));
        do { j--; } while (comp(*pivot, *j));
 
        if (i >= j) {
            swap(*pivot, *j);
            return j;
        }
            
        swap(*i, *j);

        if (i == pivot) {
            pivot = j;
        } else if (j == pivot) {
            pivot = i;
        }
    }
}

template<typename T, typename Compare>
void quickSortOptimized(T* first, T* last, Compare comp) {
    const int INSERTION_SORT_THRESHOLD = 128;

    if (last - first < INSERTION_SORT_THRESHOLD) {
        insertionSort(first, last, comp);
        return;
    }

    while (first < last) {
        T* pivot = partition(first, last, comp);

        if (pivot - first < last - pivot) {
            quickSortOptimized(first, pivot - 1, comp);
            first = pivot + 1;
        } else {
            quickSortOptimized(pivot + 1, last, comp);
            last = pivot - 1;
        }
    }
}

template<typename T, typename Compare>
void quickSort(T* first, T* last, Compare comp) {
    while (first < last) {
        T* pivot = partition(first, last, comp);

        if (pivot - first < last - pivot) {
            quickSort(first, pivot - 1, comp);
            first = pivot + 1;
        }
        else {
            quickSort(pivot + 1, last, comp);
            last = pivot - 1;
        }
    }
}

template<typename T, typename Compare>
void insertionSort(T* first, T* last, Compare comp) {
    for (T* i = first + 1; i <= last; i++) {
        T* value = i;
        T* j = i;
        while (j > first && comp(*value, *(j - 1))) {
            new (j) T(std::move(*(j - 1)));
            (j - 1)->~T();
            j--;
        }

        new (j) T(std::move(*value));
    }
}