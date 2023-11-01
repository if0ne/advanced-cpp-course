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
    virtual bool checkPtr(void* p) = 0;
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

        if (checkPtr(p)) {
            FreeListNode* fh = reinterpret_cast<FreeListNode*>(p);
            fh->next = fh_;
            fh_ = fh;

#ifdef _DEBUG
            alloc_block_count_--;
#endif

            return;
        }
        
        if (next_) {
            next_->free(p);

#ifdef _DEBUG
            alloc_block_count_--;
#endif

            return;
        }

        assert(false && "Wrong address to free");
    }

    virtual bool checkPtr(void* p) override {
        FixedSizedBlockAllocator<N>* cur = this;
        bool contains = false;

        while (cur != nullptr) {
            contains |= page_ <= (char*)p && (char*)p < page_ + size_;

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

        std::cout << "=======================\n";
        std::cout << "OS Pages:\n";
        while (cur) {
            total_blocks += cur->block_count_;
            total_alloc_blocks += cur->alloc_block_count_;

            std::cout << "Address: " << (void*)cur->page_ << " Bytes:" << cur->size_ << "\n";
            cur = cur->next_;
        }
        std::cout << "Allocated blocks: " << total_alloc_blocks << "\n";
        std::cout << "Free blocks: " << total_blocks - total_alloc_blocks << "\n";

        std::cout << "=======================" << std::endl;
    }

    virtual void dumpBlocks() const override {
        assert(page_);

        const FixedSizedBlockAllocator<N>* cur = this;

        std::cout << "=======================\n";
        std::cout << "Allocated blocks:\n";
        while (cur) {
            for (size_t i = 0; i < cur->block_count_; i++) {
                if (cur->isAllocatedBlock(cur->page_ + i * N)) {
                    std::cout << "Address: " << (void*)(cur->page_ + i * N) << " Bytes: " << N << "\n";
                }
            }
            cur = cur->next_;
        }
        std::cout << "=======================" << std::endl;
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

int main() {
    FixedSizedBlockAllocator<8> allocator { 32 };
    allocator.init();
   
    char* a = (char*)allocator.alloc(8);
    *a = 'b';
    char* b = (char*)allocator.alloc(8);
    char* c = (char*)allocator.alloc(8);
    char* d = (char*)allocator.alloc(8);
    char* e = (char*)allocator.alloc(8);

    allocator.dumpStat();
    allocator.dumpBlocks();

    allocator.free(a);

    allocator.destroy();

    return 0;
}