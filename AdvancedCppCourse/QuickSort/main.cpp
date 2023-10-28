#include "Quicksort.h"

#include <cassert>
#include <algorithm>
#include <benchmark/benchmark.h>

static void quickSortBench(benchmark::State& state) {
    auto data = generate(state.range(0));
    for (auto _ : state) {
        quickSort(data.data(), data.data() + data.size() - 1, [](int a, int b) { return a < b; });
    }
}

BENCHMARK(quickSortBench)->RangeMultiplier(2)->Range(2, 1 << 10);

static void insertSortBench(benchmark::State& state) {
    auto data = generate(state.range(0));
    for (auto _ : state) {
        insertionSort(data.data(), data.data() + data.size() - 1, [](int a, int b) { return a < b; });
    }
}

BENCHMARK(insertSortBench)->RangeMultiplier(2)->Range(2, 1 << 10);

static void stdSortBench(benchmark::State& state) {
    auto data = generate(state.range(0));
    for (auto _ : state) {
        std::sort(data.begin(), data.end());
    }
}

BENCHMARK(stdSortBench)->RangeMultiplier(2)->Range(2, 1 << 10);

BENCHMARK_MAIN();