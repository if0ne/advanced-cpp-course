#pragma once
#include "IAllocator.h"
#include "FixedSizedBlockAllocator.h"
#include "CoalesceAllocator.h"
#include "OsAllocator.h"
#include <Windows.h>

#include <cassert>
#include <iostream>

class HybridAllocator final : public IAllocator {
public:
    HybridAllocator() :
        IAllocator(),
        fsa16_(kFourKb),
        fsa32_(kFourKb),
        fsa64_(kFourKb),
        fsa128_(kFourKb),
        fsa256_(kFourKb),
        fsa512_(kFourKb),
        coalesce_(kFourKb),
        os_()
    {
    }

    virtual void init() override;
    virtual void destroy() override;
    virtual void* alloc(size_t size) override;
    virtual void free(void* p) override;

    virtual ~HybridAllocator() {
    }

#ifdef _DEBUG 
    virtual void dumpStat() const override;
    virtual void dumpBlocks() const override;
#endif

private:
    const size_t kTenMb = 10 * 1024 * 1024;
    const size_t kFourKb = 4 * 1024;

    FixedSizedBlockAllocator<16> fsa16_;
    FixedSizedBlockAllocator<32> fsa32_;
    FixedSizedBlockAllocator<64> fsa64_;
    FixedSizedBlockAllocator<128> fsa128_;
    FixedSizedBlockAllocator<256> fsa256_;
    FixedSizedBlockAllocator<512> fsa512_;

    CoalesceAllocator coalesce_;

    OsAllocator os_;
};

