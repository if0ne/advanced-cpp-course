#pragma once
#include "IAllocator.h"
#include <Windows.h>

#include <cassert>
#include <iostream>

class CoalesceAllocator final : public IAllocator {
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

    virtual void init() override;
    virtual void destroy() override;
    virtual void* alloc(size_t size) override;
    virtual void free(void* p) override;
    bool checkPtr(void* p, CoalesceAllocator*& alloc);
    bool checkPtr(void* p);

    virtual ~CoalesceAllocator() {
        assert(page_ == nullptr);
    }

#ifdef _DEBUG 
    virtual void dumpStat() const override;
    virtual void dumpBlocks() const override;
#endif
private:
    FreeListNode* firstFit(size_t size);
    void getBlocksInfo(size_t* total_blocks, size_t* total_alloc) const;

    char* page_;
    FreeListNode* fh_;

    CoalesceAllocator* next_;
};

