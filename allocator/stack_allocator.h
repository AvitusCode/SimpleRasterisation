#pragma once
#include "allocator.h"
#include <cstdint>

namespace jd::mem
{
    class StackAllocator : public Allocator 
    {
    public:
        StackAllocator() = default;
        StackAllocator(const size_t totalSize);
        StackAllocator(const StackAllocator&) = delete;
        StackAllocator(StackAllocator&&) = delete;
        StackAllocator& operator=(const StackAllocator&) = delete;
        StackAllocator& operator=(StackAllocator&&) = delete;
        ~StackAllocator();

        void* allocate(const size_t size, const size_t alignment) override;
        void deallocate(void* ptr) noexcept override;
        void init() override;
        void reset() override;
        bool resize(size_t sz) override;

        void setMemTotal(size_t total);
    private:
        struct signature {
            uint8_t padding;
        };

    private:
        uint8_t* m_buffer = nullptr;
        size_t m_offset = 0;
        bool is_init = false;
    };
}