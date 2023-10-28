#include "Quicksort.h"

#include <chrono>
#include <iostream>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void warmup(int iters) {
    std::cout << "Begin Warmup\n";

    const int kWarmupItemCount = 1000;
    srand(time(nullptr));

    for (int i = 0; i < iters; ++i) {
        auto items = generate(kWarmupItemCount);
        quick_sort_optimized(items.data(), items.data() + kWarmupItemCount - 1, [](int a, int b) {
            return a < b;
        });
    }

    std::cout << "End Warmup\n";
}

template<typename Function>
double iter(Function func, std::vector<int>& data) {
    auto t1 = high_resolution_clock::now();
    func(data);
    auto t2 = high_resolution_clock::now();

    duration<double, std::micro> delta = t2 - t1;

    return delta.count();
}

template<typename Function>
double bench(Function func, int sample_size, int data_size) {
    double all_time = 0.0;

    for (int i = 0; i < sample_size; i++) {
        auto data = generate(data_size);
        all_time += iter(func, data);
    }

    return all_time / sample_size;
}

template<typename Function>
std::vector<double> bench_group(Function func, const std::vector<int>& inputs) {
    const int SAMPLE_SIZE = 10;
    std::vector<double> results;

    for (auto i : inputs) {
        results.push_back(bench(func, SAMPLE_SIZE, i));
    }

    return results;
}

int main() {
    const int kWarmupIters = 100;
    std::vector<int> inputs = { 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024 };

    warmup(kWarmupIters);

    auto quick_sort_results = bench_group([](std::vector<int>& data) {
        quick_sort(data.data(), data.data() + data.size() - 1, [](int a, int b) { return a < b; });
    }, inputs);

    auto insertion_sort_results = bench_group([](std::vector<int>& data) {
        insertion_sort(data.data(), data.data() + data.size() - 1, [](int a, int b) { return a < b; });
    }, inputs);

    std::cout << "QuickSort:\n";

    for (int i = 0; i < inputs.size(); i++) {
        std::cout << "N = " << inputs[i] << " ; Time(us) = " << quick_sort_results[i] << "\n";
    }

    std::cout << "\n===============================\n\n";

    std::cout << "Insertion Sort: \n";

    for (int i = 0; i < inputs.size(); i++) {
        std::cout << "N = " << inputs[i] << " ; Time(us) = " << insertion_sort_results[i] << "\n";
    }
}