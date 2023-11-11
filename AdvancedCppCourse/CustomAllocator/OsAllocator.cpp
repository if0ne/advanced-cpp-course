#include "OsAllocator.h"
void OsAllocator::init() {
    offset = sizeof(OsAllocator);
    if (size_ != 0) {
        page_ = (char*)VirtualAlloc(nullptr, size_ + offset, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        assert(page_);
        is_free_ = true;
    }
}

void OsAllocator::destroy() {
    assert(page_);

#ifdef _DEBUG
    if (!is_free_) {
        std::cout << "Detected memory leak. Address: " << (void*)(page_ + offset) << " Bytes: " << size_ << "\n";
    }
#endif

    if (next_) {
        next_->destroy();
    }

    VirtualFree((void*)page_, 0, MEM_RELEASE);
    page_ = nullptr;
}

void* OsAllocator::alloc(size_t size) {
    if (!page_) {
        size_ = size;
        init();
    }

    if (is_free_ && size <= size_) {
        is_free_ = false;
        return page_ + offset;
    }

    if (!next_) {
        next_ = reinterpret_cast<OsAllocator*>(page_);
        new (next_) OsAllocator();
    }

    void* mem = next_->alloc(size);

    return mem;
}

void OsAllocator::free(void* p) {
    assert(page_);

    OsAllocator* allocator = nullptr;

    if (checkPtr(p, allocator)) {
        allocator->is_free_ = true;
        return;
    }

    assert(false && "Wrong address to free");
}

bool OsAllocator::checkPtr(void* p, OsAllocator*& allocator) {
    OsAllocator* cur = this;
    bool contains = false;

    while (cur != nullptr) {
        contains |= cur->page_ + offset <= (char*)p && (char*)p < cur->page_ + cur->size_;

        if (contains) {
            break;
        }

        cur = cur->next_;
    }
    allocator = cur;

    return contains && cur->page_ + offset == (char*)p;
}

bool OsAllocator::checkPtr(void* p) {
    OsAllocator* cur = this;
    return checkPtr(p, cur);
}

#ifdef _DEBUG 
void OsAllocator::dumpStat() const {
    assert(page_);

    size_t total_blocks = 0;
    size_t total_alloc_blocks = 0;

    const OsAllocator* cur = this;

    std::cout << "OS Pages for OsAllocator:\n";
    while (cur) {
        total_blocks++;
        if (!cur->is_free_) total_alloc_blocks++;
        std::cout << "Address: " << (void*)cur->page_ << " Bytes:" << cur->size_ << "\n";
        cur = cur->next_;
    }

    std::cout << "Allocated blocks: " << total_alloc_blocks << "\n";
    std::cout << "Free blocks: " << total_blocks - total_alloc_blocks << "\n";
}

void OsAllocator::dumpBlocks() const {
    assert(page_);

    std::cout << "Allocated blocks for OsAllocator:\n";
    const OsAllocator* cur = this;

    while (cur) {
        std::cout << "Address: " << (void*)(cur->page_ + offset) << " Bytes:" << cur->size_ << "\n";
        cur = cur->next_;
    }
}
#endif