#include "stack_allocator.h"
#include <memory>

namespace jd::mem
{
	void StackAllocator::setMemTotal(size_t total)
	{
		if (!is_init) {
			m_totalSize = total;
		}
	}

	StackAllocator::StackAllocator(const size_t totalSize) : Allocator(totalSize), m_buffer(nullptr), m_offset(0) {
		init();
	}

	StackAllocator::~StackAllocator()
	{
		free(m_buffer);
		m_buffer = nullptr;
	}

	bool StackAllocator::resize(size_t sz) {
		// @Deprecated
		return true;
	}

	void* StackAllocator::allocate(const size_t size, const size_t alignment)
	{
		uint8_t* m_ptr = m_buffer + m_offset;
		const size_t padding = sizeof(signature) + alignment - ((size + sizeof(signature)) % alignment);

		if (m_offset + padding + size > m_totalSize) {
			return nullptr;
		}

		m_offset += padding;

		uint8_t* next_address = m_ptr + padding;
		signature* signature_adress = reinterpret_cast<signature*>(next_address - sizeof(signature));
		signature_adress->padding = static_cast<uint8_t>(padding);

		m_offset += size;
		m_used = m_offset;

		return next_address;
	}

	void StackAllocator::deallocate(void* ptr) noexcept
	{
		uint8_t* m_ptr = static_cast<uint8_t*>(ptr);
		signature* signature_adress = reinterpret_cast<signature*>(m_ptr - sizeof(signature));

		m_offset = static_cast<size_t>(m_ptr - m_buffer) - signature_adress->padding;
		m_used = m_offset;
	}

	void StackAllocator::init()
	{
		if (is_init) {
			return;
		}

		if (m_buffer != nullptr) {
			free(m_buffer);
		}

		m_buffer = (uint8_t*)malloc(m_totalSize);
		if (m_buffer == nullptr) {
			throw std::bad_alloc();
		}
		m_offset = 0;
		is_init = true;
	}

	void StackAllocator::reset() {
		m_offset = m_used = 0;
	}
}