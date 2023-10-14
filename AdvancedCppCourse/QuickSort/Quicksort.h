#pragma once
#include <utility>

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
void partition(T* first, T* last, T* pivot, Compare comp) {
    while (first <= last) {
        while (comp(*first, *pivot)) first++;
        while (comp(*pivot, *last)) last--;

        if (first <= last) {
            swap(*first, *last);

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
void quick_sort_modified(T* first, T* last, Compare comp) {
    const int INSERTION_SORT_THRESHOLD = 64;
    if (first >= last) {
        return;
    }

    if (last - first < INSERTION_SORT_THRESHOLD) {
        insertion_sort(first, last, comp);
        return;
    }

    T* pivot = select_pivot(first, last, comp);
    partition(first, last, pivot, comp);
    
    if (pivot - first < last - pivot) {
        quick_sort_iterative(pivot + 1, last, comp);
        quick_sort(first, pivot - 1, comp);
    } else {
        quick_sort_iterative(first, pivot - 1, comp);
        quick_sort(pivot + 1, last, comp);
    }
}

template<typename T, typename Compare>
void quick_sort_iterative(T* first, T* last, Compare comp) {
    T** stack = new T*[last - first + 1];

    int top = 0;

    stack[top++] = first;
    stack[top++] = last;

    while (top > 0) {
        T* l = stack[--top];
        T* f = stack[--top];

        T* p = select_pivot(f, l, comp);
        partition(f, l, p, comp);

        if (p - 1 > f) {
            stack[top++] = f;
            stack[top++] = p - 1;
        }

        if (p + 1 < l) {
            stack[top++] = p + 1;
            stack[top++] = l;
        }
    }

    delete[] stack;
}

template<typename T, typename Compare>
void quick_sort(T* first, T* last, Compare comp) {
    if (first >= last) {
        return;
    }

    T* pivot = select_pivot(first, last, comp);
    partition(first, last, pivot, comp);

    quick_sort(first, pivot - 1, comp);
    quick_sort(pivot + 1, last, comp);
}

template<typename T, typename Compare>
void insertion_sort(T* first, T* last, Compare comp) {
    for (T* i = first + 1; i < last; i++) {
        T* j = i;
        while (j > first && comp(*j, *(j - 1))) {
            swap(*j, *(j - 1));
            j--;
        }
    }
}