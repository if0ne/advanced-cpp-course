#pragma once
#include "IAllocator.h"
#include <Windows.h>

#include <cassert>
#include <iostream>

class OsAllocator final : public IAllocator {
public:
    OsAllocator() : IAllocator() {
        page_ = nullptr;
        next_ = nullptr;
        is_free_ = false;
    }

    virtual void init() override;
    virtual void destroy() override;
    virtual void* alloc(size_t size) override;
    virtual void free(void* p) override;
    bool checkPtr(void* p, OsAllocator*& allocator);
    bool checkPtr(void* p);

    virtual ~OsAllocator() {
        assert(page_ == nullptr);
    }

#ifdef _DEBUG 
    virtual void dumpStat() const override;
    virtual void dumpBlocks() const override;
#endif
private:
    char* page_;
    bool is_free_;

    OsAllocator* next_;
};