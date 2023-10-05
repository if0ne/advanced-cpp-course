#include "Array.h"

#include <iostream>

int main() {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);
    a.insert(4);

    for (auto& i : a) {
        std::cout << i << std::endl;
        i = 5;
    }

    for (auto& i : a) {
        std::cout << i << std::endl;
    }

    return 0;
}