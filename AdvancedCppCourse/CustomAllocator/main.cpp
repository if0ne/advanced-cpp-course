#include <Windows.h>
#include <cassert>
#include <iostream>

class IAllocator {
public:
    IAllocator(size_t size) : size_(size) {}
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void* alloc(size_t size) = 0;
    virtual void free(void* p) = 0;
#ifdef _DEBUG 
    virtual void dumpStat() const = 0;
    virtual void dumpBlocks() const = 0;
#endif
protected:
    size_t size_;
};

template<size_t N>
class FixedSizedBlockAllocator final: public IAllocator {
private:
    struct FreeListNode {
        FreeListNode* next;
    };

public:
    FixedSizedBlockAllocator<N>(size_t size) : IAllocator(size) {
        page_ = nullptr;
        fh_ = nullptr;
        next_ = nullptr;

#ifdef _DEBUG
        block_count_ = 0;
        alloc_block_count_ = 0;
#endif 
    }

    virtual void init() override {
        page_ = (char*)VirtualAlloc(nullptr, size_, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        assert(page_);

        fh_ = reinterpret_cast<FreeListNode*>(page_);
        size_t num = size_ / N;

#ifdef _DEBUG
        block_count_ = num;
#endif 

        FreeListNode* cur = fh_;
        for (size_t i = 0; i < num - 1; i++) {
            cur->next = reinterpret_cast<FreeListNode*>(page_ + (i + 1) * N);
            cur = cur->next;
        }

        cur->next = nullptr;
    }

    virtual void destroy() override {
        assert(page_);

#ifdef _DEBUG
        for (size_t i = 0; i < block_count_; i++) {
            if (isAllocatedBlock(page_ + i * N)) {
                std::cout << "Detected memory leak. Address: " << (void*)(page_ + i * N) << " Bytes: " << N << "\n";
            }
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

    virtual void* alloc(size_t size) override {
        assert(page_);
        assert(size <= N);

        void* mem = nullptr;

        if (fh_) {
            mem = reinterpret_cast<void*>(fh_);
            fh_ = fh_->next;
#ifdef _DEBUG
            alloc_block_count_++;
#endif
            return mem;
        }

        if (next_ == nullptr) {
            next_ = new FixedSizedBlockAllocator<N>(size_);
            next_->init();
        }
            
        mem = next_->alloc(size);

        return mem;
    }

    virtual void free(void* p) override {
        assert(page_);
        FixedSizedBlockAllocator<N>* allocator;
        if (checkPtr(p, allocator)) {
            FreeListNode* fh = reinterpret_cast<FreeListNode*>(p);
            allocator->fh->next = fh_;
            allocator->fh_ = fh;

#ifdef _DEBUG
            allocator->alloc_block_count_--;
#endif

            return;
        }

        assert(false && "Wrong address to free");
    }

    bool checkPtr(void* p, FixedSizedBlockAllocator<N>* alloc) {
        FixedSizedBlockAllocator<N>* cur = this;
        bool contains = false;

        while (cur != nullptr) {
            contains |= cur->page_ <= (char*)p && (char*)p < cur->page_ + cur->size_;

            if (contains) {
                break;
            }

            cur = cur->next_;
        }
        alloc = cur;
        bool is_start_block = false;

        if (cur != nullptr) {
            is_start_block = ((char*)p - cur->page_) % N == 0;
        }

        return contains && is_start_block;
    }

    virtual ~FixedSizedBlockAllocator<N>() {
        assert(page_ == nullptr);
    }
#ifdef _DEBUG 
    virtual void dumpStat() const override {
        assert(page_);

        size_t total_blocks = 0;
        size_t total_alloc_blocks = 0;

        const FixedSizedBlockAllocator<N>* cur = this;

        std::cout << "OS Pages:\n";
        while (cur) {
            total_blocks += cur->block_count_;
            total_alloc_blocks += cur->alloc_block_count_;

            std::cout << "Address: " << (void*)cur->page_ << " Bytes:" << cur->size_ << "\n";
            cur = cur->next_;
        }
        std::cout << "Allocated blocks: " << total_alloc_blocks << "\n";
        std::cout << "Free blocks: " << total_blocks - total_alloc_blocks << "\n";
    }

    virtual void dumpBlocks() const override {
        assert(page_);

        const FixedSizedBlockAllocator<N>* cur = this;

        std::cout << "Allocated blocks:\n";
        while (cur) {
            for (size_t i = 0; i < cur->block_count_; i++) {
                if (cur->isAllocatedBlock(cur->page_ + i * N)) {
                    std::cout << "Address: " << (void*)(cur->page_ + i * N) << " Bytes: " << N << "\n";
                }
            }
            cur = cur->next_;
        }
    }
#endif
private:
    bool isAllocatedBlock(char* p) const {
        FreeListNode* cur = fh_;

        while (cur) {
            char* pp = reinterpret_cast<char*>(cur);

            if (pp == p) {
                return false;
            }

            cur = cur->next;
        }

        return true;
    }

    char* page_;
    FreeListNode* fh_;

    FixedSizedBlockAllocator<N>* next_;

#ifdef _DEBUG
    size_t block_count_;
    size_t alloc_block_count_;
#endif
};

class CoalesceAllocator final: public IAllocator {
private:
    struct FreeListNode {
        FreeListNode* next;
        FreeListNode* prev;
        size_t size;
        size_t is_free;
    };

    const size_t kHeader = sizeof(FreeListNode);

public:
    CoalesceAllocator(size_t size) : IAllocator(size) {
        page_ = nullptr;
        fh_ = nullptr;
        next_ = nullptr;
    }

    virtual void init() override {
        page_ = (char*)VirtualAlloc(nullptr, size_, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        assert(page_);

        fh_ = reinterpret_cast<FreeListNode*>(page_);
        fh_->is_free = 1;
        fh_->size = size_;
        fh_->prev = nullptr;
        fh_->next = nullptr;
    }

    virtual void destroy() override {
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

    virtual void* alloc(size_t size) override {
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

    virtual void free(void* p) override {
        assert(page_);

        CoalesceAllocator* allocator = nullptr;

        if (checkPtr(p, allocator)) {
            FreeListNode* fh = reinterpret_cast<FreeListNode*>((char*)p - kHeader);

            if (fh->prev && fh->next) {
                if (fh->prev->is_free && fh->next->is_free) {
                    fh->prev->next = fh->next->next;
                    fh->prev->size += 2 * kHeader + fh->size + fh->next->size;
                } else if (fh->prev->is_free) {
                    fh->prev->next = fh->next;
                    fh->prev->size += kHeader + fh->size;
                } else if (fh->next->is_free) {
                    fh->size += kHeader + fh->next->size;
                    fh->next = fh->next->next;
                    fh->is_free = 1;
                } else {
                    fh->is_free = 1;
                }
            } else if (fh->prev) {
                if (fh->prev->is_free) {
                    fh->prev->next = nullptr;
                    fh->prev->size += kHeader + fh->size;
                } else {
                    fh->is_free = 1;
                }
            } else if (fh->next) {
                if (fh->next->is_free) {
                    fh->size += kHeader + fh->next->size;
                    fh->next = fh->next->next;
                    fh->is_free = 1;
                } else {
                    fh->is_free = 1;
                }
            } else {
                fh->is_free = 1;
            }

            return;
        }

        assert(false && "Wrong address to free");
    }

    bool checkPtr(void* p, CoalesceAllocator* alloc) {
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

    virtual ~CoalesceAllocator() {
        assert(page_ == nullptr);
    }
#ifdef _DEBUG 
    virtual void dumpStat() const override {
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

    virtual void dumpBlocks() const override {
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
private:
    FreeListNode* firstFit(size_t size) {
        FreeListNode* cur = fh_;

        while (cur) {
            if (cur->is_free == 1 && cur->size >= size + kHeader) {
                return cur;
            }
            cur = cur->next;
        }

        return nullptr;
    }

    void getBlocksInfo(size_t* total_blocks, size_t* total_alloc) const {
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

    char* page_;
    FreeListNode* fh_;

    CoalesceAllocator* next_;
};

int main() {

    return 0;
}