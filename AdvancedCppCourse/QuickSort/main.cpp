#include "Quicksort.h"

int main() {
    int a[] = { 5, 4, 6, 7, 3, 2, 1};

    quick_sort(a, a + 6, [](int a, int b) { return a < b; });
}