#include "HybridAllocator.h"

int main() {
    HybridAllocator allocator{};
    allocator.init();
    int* pi = (int*)allocator.alloc(sizeof(int));
    double* pd = (double*)allocator.alloc(sizeof(double));
    int* pa = (int*)allocator.alloc(10 * sizeof(int));
    int* ps = (int*)allocator.alloc(11 * 1024 * 1024);
    allocator.dumpStat();
    allocator.dumpBlocks();
    allocator.free(pa);
    allocator.free(pd);
    allocator.free(pi);
    allocator.free(ps);
    allocator.destroy();

    return 0;
}