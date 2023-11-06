#include "CoalesceAllocator.h"
void CoalesceAllocator::init() {
    page_ = (char*)VirtualAlloc(nullptr, size_, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(page_);

    fh_ = reinterpret_cast<FreeListNode*>(page_);
    fh_->is_free = 1;
    fh_->size = size_;
    fh_->prev = nullptr;
    fh_->next = nullptr;
}

void CoalesceAllocator::destroy() {
    assert(page_);

#ifdef _DEBUG
    FreeListNode* cur = fh_;

    while (cur) {
        if (cur->is_free == 0) {
            std::cout << "Detected memory leak. Address: " << (void*)(cur) << " Bytes: " << cur->size << "\n";
        }

        cur = cur->next;
    }
#endif

    if (next_) {
        next_->destroy();
        delete next_;
    }

    VirtualFree((void*)page_, 0, MEM_RELEASE);
    page_ = nullptr;
    fh_ = nullptr;
}

void* CoalesceAllocator::alloc(size_t size) {
    assert(page_);
    assert(size + kHeader <= size_);
    FreeListNode* free = firstFit(size);

    if (!free) {
        if (next_ == nullptr) {
            next_ = new CoalesceAllocator(size_);
            next_->init();
        }

        free = reinterpret_cast<FreeListNode*>(next_->alloc(size));

        return free;
    }

    FreeListNode* next = nullptr;

    if (free->size - size > kHeader) {
        next = reinterpret_cast<FreeListNode*>(reinterpret_cast<char*>(free) + kHeader + size);
        next->prev = free;
        next->size = free->size - size - kHeader;
        next->is_free = 1;
    }

    free->is_free = 0;
    free->size = size;
    free->next = next;

    return reinterpret_cast<char*>(free) + kHeader;
}

void CoalesceAllocator::free(void* p) {
    assert(page_);

    CoalesceAllocator* allocator = nullptr;

    if (checkPtr(p, allocator)) {
        FreeListNode* fh = reinterpret_cast<FreeListNode*>((char*)p - kHeader);

        if (fh->prev && fh->next) {
            if (fh->prev->is_free && fh->next->is_free) {
                fh->prev->next = fh->next->next;
                fh->prev->size += 2 * kHeader + fh->size + fh->next->size;
            }
            else if (fh->prev->is_free) {
                fh->prev->next = fh->next;
                fh->prev->size += kHeader + fh->size;
            }
            else if (fh->next->is_free) {
                fh->size += kHeader + fh->next->size;
                fh->next = fh->next->next;
                fh->is_free = 1;
            }
            else {
                fh->is_free = 1;
            }
        }
        else if (fh->prev) {
            if (fh->prev->is_free) {
                fh->prev->next = nullptr;
                fh->prev->size += kHeader + fh->size;
            }
            else {
                fh->is_free = 1;
            }
        }
        else if (fh->next) {
            if (fh->next->is_free) {
                fh->size += kHeader + fh->next->size;
                fh->next = fh->next->next;
                fh->is_free = 1;
            }
            else {
                fh->is_free = 1;
            }
        }
        else {
            fh->is_free = 1;
        }

        return;
    }

    assert(false && "Wrong address to free");
}

bool CoalesceAllocator::checkPtr(void* p, CoalesceAllocator*& alloc) {
    CoalesceAllocator* cur = this;
    bool contains = false;

    while (cur != nullptr) {
        contains |= cur->page_ <= (char*)p && (char*)p < cur->page_ + cur->size_;

        if (contains) {
            break;
        }

        cur = cur->next_;
    }
    alloc = cur;

    bool is_allocated_block = false;

    if (cur != nullptr) {
        is_allocated_block = reinterpret_cast<FreeListNode*>((char*)p - kHeader)->is_free == 0;
    }

    return contains && is_allocated_block;
}

bool CoalesceAllocator::checkPtr(void* p) {
    CoalesceAllocator* cur = this;
    return checkPtr(p, cur);
}

CoalesceAllocator::FreeListNode* CoalesceAllocator::firstFit(size_t size) {
    FreeListNode* cur = fh_;

    while (cur) {
        if (cur->is_free == 1 && cur->size >= size + kHeader) {
            return cur;
        }
        cur = cur->next;
    }

    return nullptr;
}

void CoalesceAllocator::getBlocksInfo(size_t* total_blocks, size_t* total_alloc) const {
    FreeListNode* cur = fh_;

    while (cur) {
        (*total_blocks)++;
        if (cur->is_free == 0) {
            (*total_alloc)++;
        }

        cur = cur->next;
    }

    if (next_) {
        next_->getBlocksInfo(total_blocks, total_alloc);
    }
}

#ifdef _DEBUG 
void CoalesceAllocator::dumpStat() const {
    assert(page_);

    size_t total_blocks = 0;
    size_t total_alloc_blocks = 0;

    const CoalesceAllocator* cur = this;

    std::cout << "OS Pages:\n";
    while (cur) {

        std::cout << "Address: " << (void*)cur->page_ << " Bytes:" << cur->size_ << "\n";
        cur = cur->next_;
    }

    getBlocksInfo(&total_blocks, &total_alloc_blocks);
    std::cout << "Allocated blocks: " << total_alloc_blocks << "\n";
    std::cout << "Free blocks: " << total_blocks - total_alloc_blocks << "\n";
}

void CoalesceAllocator::dumpBlocks() const {
    assert(page_);

    std::cout << "Allocated blocks:\n";
    FreeListNode* cur = fh_;

    while (cur) {
        if (cur->is_free == 0) {
            std::cout << "Address: " << (void*)(cur) << " Bytes : " << cur->size << "\n";
        }

        cur = cur->next;
    }

    if (next_) {
        next_->dumpBlocks();
    }
}
#endif