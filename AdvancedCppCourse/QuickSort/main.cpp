#include "Quicksort.h"
#include <chrono>
#include <iostream>
#include <vector>

template <
    class result_t = std::chrono::microseconds,
    class clock_t = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start)
{
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

std::vector<int> generate(int n) {
    std::vector<int> foo;

    for (int i = 0; i < n; i++) {
        foo.push_back(n - i);
    }

    return foo;
}

int main() {
    auto a = generate(10000);
    auto b = generate(10000);

    auto start = std::chrono::steady_clock::now();
    quick_sort(a.data(), a.data() + a.size() - 1, [](int a, int b) { return a < b; });
    std::cout << "Elapsed(us)=" << since(start).count() << std::endl;

    start = std::chrono::steady_clock::now();
    quick_sort_modified(b.data(), b.data() + b.size() - 1, [](int a, int b) { return a < b; });
    std::cout << "Elapsed(us)=" << since(start).count() << std::endl;

    int c = 0;
}