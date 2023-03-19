#pragma once
#include <cstddef> 

namespace jd::mem
{
    class Allocator
    {
    public:
        Allocator(const size_t totalSize = 0) : m_totalSize(totalSize), m_used(0) { }
        virtual ~Allocator() { m_totalSize = 0; }

        virtual void* allocate(const size_t size, const size_t alignment = 0) = 0;
        virtual void deallocate(void* ptr) noexcept = 0;
        virtual void init() = 0;
        virtual void reset() = 0;
        virtual bool resize(size_t sz) = 0;
        
        size_t memUse() const {
            return m_used;
        }
        size_t memTotal() const {
            return m_totalSize;
        }
    protected:
        size_t m_totalSize;
        size_t m_used;
    };
}