#pragma once
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
    IAllocator() : size_(0) {}
    size_t size_;
};