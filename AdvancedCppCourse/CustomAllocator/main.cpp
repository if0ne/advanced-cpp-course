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
            fh->next = allocator->fh_;
            allocator->fh_ = fh;

#ifdef _DEBUG
            allocator->alloc_block_count_--;
#endif

            return;
        }

        assert(false && "Wrong address to free");
    }

    bool checkPtr(void* p, FixedSizedBlockAllocator<N>*& alloc) {
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

    bool checkPtr(void* p) {
        FixedSizedBlockAllocator<N>* cur = this;
        bool contains = false;

        while (cur != nullptr) {
            contains |= cur->page_ <= (char*)p && (char*)p < cur->page_ + cur->size_;

            if (contains) {
                break;
            }

            cur = cur->next_;
        }
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

    bool checkPtr(void* p, CoalesceAllocator*& alloc) {
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

    bool checkPtr(void* p) {
        CoalesceAllocator* cur = this;
        bool contains = false;

        while (cur != nullptr) {
            contains |= cur->page_ <= (char*)p && (char*)p < cur->page_ + cur->size_;

            if (contains) {
                break;
            }

            cur = cur->next_;
        }

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

class OsAllocator final : public IAllocator {
public:
    OsAllocator(size_t size) : IAllocator(size) {
        page_ = nullptr;
        next_ = nullptr;
        is_free_ = false;
    }

    virtual void init() override {
        page_ = (char*)VirtualAlloc(nullptr, size_, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        assert(page_);
        is_free_ = true;
    }

    virtual void destroy() override {
        assert(page_);

#ifdef _DEBUG
        if (!is_free_) {
            std::cout << "Detected memory leak. Address: " << (void*)(page_) << " Bytes: " << size_ << "\n";
        }
#endif

        if (next_) {
            next_->destroy();
            delete next_;
        }

        VirtualFree((void*)page_, 0, MEM_RELEASE);
        page_ = nullptr;
    }

    virtual void* alloc(size_t size) override {
        assert(page_);

        if (is_free_ && size <= size_) {
            is_free_ = false;
            return page_;
        }

        if (!next_) {
            next_ = new OsAllocator(size);
            next_->init();
        }

        void* mem = next_->alloc(size);

        return mem;
    }

    virtual void free(void* p) override {
        assert(page_);

        OsAllocator* allocator = nullptr;

        if (checkPtr(p, allocator)) {
            allocator->is_free_ = true;
            return;
        }

        assert(false && "Wrong address to free");
    }

    bool checkPtr(void* p, OsAllocator*& allocator) {
        OsAllocator* cur = this;
        bool contains = false;

        while (cur != nullptr) {
            contains |= cur->page_ <= (char*)p && (char*)p < cur->page_ + cur->size_;

            if (contains) {
                break;
            }

            cur = cur->next_;
        }
        allocator = cur;

        return contains && cur->page_ == (char*)p;
    }

    bool checkPtr(void* p) {
        OsAllocator* cur = this;
        bool contains = false;

        while (cur != nullptr) {
            contains |= cur->page_ <= (char*)p && (char*)p < cur->page_ + cur->size_;

            if (contains) {
                break;
            }

            cur = cur->next_;
        }

        return contains && cur->page_ == (char*)p;
    }

    virtual ~OsAllocator() {
        assert(page_ == nullptr);
    }
#ifdef _DEBUG 
    virtual void dumpStat() const override {
        assert(page_);

        size_t total_blocks = 0;
        size_t total_alloc_blocks = 0;

        const OsAllocator* cur = this;

        std::cout << "OS Pages:\n";
        while (cur) {
            total_blocks++;
            if (!cur->is_free_) total_alloc_blocks++;
            std::cout << "Address: " << (void*)cur->page_ << " Bytes:" << cur->size_ << "\n";
            cur = cur->next_;
        }

        std::cout << "Allocated blocks: " << total_alloc_blocks << "\n";
        std::cout << "Free blocks: " << total_blocks - total_alloc_blocks << "\n";
    }

    virtual void dumpBlocks() const override {
        assert(page_);

        std::cout << "Allocated blocks:\n";
        const OsAllocator* cur = this;

        while (cur) {
            std::cout << "Address: " << (void*)cur->page_ << " Bytes:" << cur->size_ << "\n";
            cur = cur->next_;
        }
    }
#endif
private:
    char* page_;
    bool is_free_;

    OsAllocator* next_;
};

class HybridAllocator final : public IAllocator {
public:
    HybridAllocator(size_t default_allocator_sizes) : 
        IAllocator(default_allocator_sizes),
        fsa16_(default_allocator_sizes),
        fsa32_(default_allocator_sizes),
        fsa64_(default_allocator_sizes),
        fsa128_(default_allocator_sizes),
        fsa256_(default_allocator_sizes),
        fsa512_(default_allocator_sizes),
        coalesce_(default_allocator_sizes),
        os_(kTenMb)
    {
    }

    virtual void init() override {
        fsa16_.init();
        fsa32_.init();
        fsa64_.init();
        fsa128_.init();
        fsa256_.init();
        fsa512_.init();
        coalesce_.init();
        os_.init();
    }

    virtual void destroy() override {
        fsa16_.destroy();
        fsa32_.destroy();
        fsa64_.destroy();
        fsa128_.destroy();
        fsa256_.destroy();
        fsa512_.destroy();
        coalesce_.destroy();
        os_.destroy();
    }

    virtual void* alloc(size_t size) override {
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

    virtual void free(void* p) override {
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

    virtual ~HybridAllocator() {
    }
#ifdef _DEBUG 
    virtual void dumpStat() const override {
        fsa16_.dumpStat();
        fsa32_.dumpStat();
        fsa64_.dumpStat();
        fsa128_.dumpStat();
        fsa256_.dumpStat();
        fsa512_.dumpStat();
        coalesce_.dumpStat();
        os_.dumpStat();
    }

    virtual void dumpBlocks() const override {
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
private:
    const size_t kTenMb = 10 * 1024 * 1024;

    FixedSizedBlockAllocator<16> fsa16_;
    FixedSizedBlockAllocator<32> fsa32_;
    FixedSizedBlockAllocator<64> fsa64_;
    FixedSizedBlockAllocator<128> fsa128_;
    FixedSizedBlockAllocator<256> fsa256_;
    FixedSizedBlockAllocator<512> fsa512_;

    CoalesceAllocator coalesce_;

    OsAllocator os_;
};

int main() {
    HybridAllocator allocator{ 2048 };
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