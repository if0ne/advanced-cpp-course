#pragma once
#include <utility>
#include <vector>
#include <cstdlib>
#include <ctime>

std::vector<int> generate(int n) {
    //srand(time(nullptr));
    const int kMaxInt = 1 << 20;
    std::vector<int> foo;

    for (int i = 0; i < n; i++) {
        foo.push_back(rand() % kMaxInt);
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
T* select_pivot(T* first, T* last, Compare comp) {
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
    T* pivot = select_pivot(first, last, comp);
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
void quick_sort_optimized(T* first, T* last, Compare comp) {
    const int INSERTION_SORT_THRESHOLD = 128;

    if (last - first < INSERTION_SORT_THRESHOLD) {
        insertion_sort(first, last, comp);
        return;
    }

    while (first < last) {
        T* pivot = partition(first, last, comp);

        if (pivot - first < last - pivot) {
            quick_sort_optimized(first, pivot - 1, comp);
            first = pivot + 1;
        } else {
            quick_sort_optimized(pivot + 1, last, comp);
            last = pivot - 1;
        }
    }
}

template<typename T, typename Compare>
void quick_sort(T* first, T* last, Compare comp) {
    while (first < last) {
        T* pivot = partition(first, last, comp);

        if (pivot - first < last - pivot) {
            quick_sort(first, pivot - 1, comp);
            first = pivot + 1;
        }
        else {
            quick_sort(pivot + 1, last, comp);
            last = pivot - 1;
        }
    }
}

template<typename T, typename Compare>
void insertion_sort(T* first, T* last, Compare comp) {
    for (T* i = first + 1; i <= last; i++) {
        T* j = i;
        while (j > first && comp(*j, *(j - 1))) {
            swap(*j, *(j - 1));
            j--;
        }
    }
}