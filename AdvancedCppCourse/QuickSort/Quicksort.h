#pragma once
#include <utility>

template<typename T>
void swap(T* left, T* right) {
    auto temp = std::move(left);
    new (left) T(std::move(right));
    new (right) T(std::move(&temp));
}

template<typename T, typename Compare>
T* select_pivot(T* first, T* last, Compare comp) {
    T& first_pivot = *first;
    T& last_pivot = *last;
    T& middle_pivot = *(first + (last - first) / 2);

    if (comp(first_pivot, middle_pivot)) {
        swap(&first_pivot, &middle_pivot);
    }

    if (comp(middle_pivot, last_pivot)) {
        swap(&middle_pivot, &last_pivot);
    }

    if (comp(first_pivot, middle_pivot)) {
        swap(&first_pivot, &middle_pivot);
    }

    return &middle_pivot;
}

template<typename T, typename Compare>
void partition(T* first, T* last, T* pivot, Compare comp) {
    while (first <= last) {
        while (comp(*first, *pivot)) first++;
        while (comp(*pivot, *last)) last--;

        if (first <= last) {
            swap(first, last);

            if (first == pivot) {
                pivot = last;
                first++;
            } else if (last == pivot) {
                pivot = first;
                last--;
            } else {
                first++;
                last--;
            }
        }
    }
}

template<typename T, typename Compare>
void quick_sort(T* first, T* last, Compare comp) {
    if (first >= last) {
        return;
    }

    T* pivot = select_pivot(first, last, comp);
    partition(first, last, pivot, comp);
    
    quick_sort(first, pivot, comp);
    quick_sort(pivot + 1, last, comp);
}

template<typename T, typename Compare>
void insertion_sort(T* first, T* last, Compare comp) {
    
}