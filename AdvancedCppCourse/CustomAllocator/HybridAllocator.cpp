#include "HybridAllocator.h"
void HybridAllocator::init() {
    fsa16_.init();
    fsa32_.init();
    fsa64_.init();
    fsa128_.init();
    fsa256_.init();
    fsa512_.init();
    coalesce_.init();
    os_.init();
}
 
void HybridAllocator::destroy() {
    fsa16_.destroy();
    fsa32_.destroy();
    fsa64_.destroy();
    fsa128_.destroy();
    fsa256_.destroy();
    fsa512_.destroy();
    coalesce_.destroy();
    os_.destroy();
}

void* HybridAllocator::alloc(size_t size) {
    if (size <= 16) {
        return fsa16_.alloc(size);
    }

    if (size <= 32) {
        return fsa32_.alloc(size);
    }

    if (size <= 64) {
        return fsa64_.alloc(size);
    }

    if (size <= 128) {
        return fsa128_.alloc(size);
    }

    if (size <= 256) {
        return fsa256_.alloc(size);
    }

    if (size <= 512) {
        return fsa512_.alloc(size);
    }

    if (size < kTenMb) {
        return coalesce_.alloc(size);
    }

    return os_.alloc(size);
}

void HybridAllocator::free(void* p) {
    if (fsa16_.checkPtr(p)) {
        fsa16_.free(p);
        return;
    }

    if (fsa32_.checkPtr(p)) {
        fsa32_.free(p);
        return;
    }

    if (fsa64_.checkPtr(p)) {
        fsa64_.free(p);
        return;
    }

    if (fsa128_.checkPtr(p)) {
        fsa128_.free(p);
        return;
    }

    if (fsa256_.checkPtr(p)) {
        fsa256_.free(p);
        return;
    }

    if (fsa512_.checkPtr(p)) {
        fsa512_.free(p);
        return;
    }

    if (coalesce_.checkPtr(p)) {
        coalesce_.free(p);
        return;
    }

    if (os_.checkPtr(p)) {
        os_.free(p);
        return;
    }

    assert(false && "Wrong address to free");
}

#ifdef _DEBUG 
void HybridAllocator::dumpStat() const {
    fsa16_.dumpStat();
    fsa32_.dumpStat();
    fsa64_.dumpStat();
    fsa128_.dumpStat();
    fsa256_.dumpStat();
    fsa512_.dumpStat();
    coalesce_.dumpStat();
    os_.dumpStat();
}

void HybridAllocator::dumpBlocks() const {
    fsa16_.dumpBlocks();
    fsa32_.dumpBlocks();
    fsa64_.dumpBlocks();
    fsa128_.dumpBlocks();
    fsa256_.dumpBlocks();
    fsa512_.dumpBlocks();
    coalesce_.dumpBlocks();
    os_.dumpBlocks();
}
#endif