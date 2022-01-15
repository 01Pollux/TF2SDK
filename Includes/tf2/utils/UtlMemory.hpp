#pragma once

#include <memory>
#include <tf2/config.hpp>
#include "UtlAlloc.hpp"

TF2_NAMESPACE_BEGIN(::utils);

//-----------------------------------------------------------------------------
// The UtlMemory class:
// A growable memory class which doubles in capacity by default.
//-----------------------------------------------------------------------------
template<class _Ty, typename _IterTy = uint32_t>
class UtlMemory
{
public:
	static constexpr int GrowType_Internal = 0;
	static constexpr int GrowType_External_Const = -1;
	static constexpr int GrowType_External_Mutable = -2;
	using value_type = _Ty;

	using reference = value_type&;
	using const_reference = const value_type&;

	using pointer = value_type*;
	using const_pointer = const value_type*;

	using iterator = pointer;
	using const_iterator = const_pointer;

	// constructor, destructor
	UtlMemory(int nGrowSize = GrowType_Internal, uint32_t nInitSize = 0);
	UtlMemory(pointer pMemory, uint32_t numElements);
	UtlMemory(const_pointer pMemory, uint32_t numElements);
	~UtlMemory();

	// Set the capacity by which the memory grows
	void init(int nGrowSize = GrowType_Internal, uint32_t nInitSize = 0);

	class iterator_t
	{
	public:
		iterator_t(_IterTy _IterTy) : index(_IterTy) { }
		_IterTy index;

		bool operator==(const iterator_t it) const { return index == it.index; }
		bool operator!=(const iterator_t it) const { return index != it.index; }
	};
	[[nodiscard]] iterator_t first() const { return iterator_t(is_valid_index(0) ? 0 : invalid_index()); }
	[[nodiscard]] iterator_t next(const iterator_t& it) const { return iterator_t(is_valid_index(it.index + 1) ? it.index + 1 : invalid_index()); }
	[[nodiscard]] _IterTy get_index(const iterator_t& it) const { return it.index; }
	[[nodiscard]] bool is_index_after(_IterTy _IterTy, const iterator_t& it) const { return _IterTy > it.index; }
	[[nodiscard]] bool is_valid_iterator(const iterator_t& it) const { return is_valid_index(it.index); }
	[[nodiscard]] iterator_t invalid_iterator() const { return iterator_t(invalid_index()); }

	// element access
	[[nodiscard]] reference operator[](_IterTy _IterTy);
	[[nodiscard]] const_reference operator[](_IterTy _IterTy) const;
	[[nodiscard]] reference at(_IterTy _IterTy);
	[[nodiscard]] const_reference at(_IterTy _IterTy) const;

	// Can we use this index?
	[[nodiscard]] bool is_valid_index(_IterTy _IterTy) const;

	// Specify the invalid ('null') index that we'll only return on failure
	[[nodiscard]] static _IterTy invalid_index() { return static_cast<_IterTy>(-1); }

	// Gets the base address (can change when adding elements!)
	[[nodiscard]] pointer data();
	[[nodiscard]] const_pointer data() const;

	// Attaches the buffer to external memory....
	void set_external_buffer(pointer pMemory, uint32_t numElements);
	void set_external_buffer(const_pointer pMemory, uint32_t numElements);
	// Takes ownership of the passed memory, including freeing it when this buffer is destroyed.
	void take_ownership(pointer pMemory, uint32_t nSize);

	// Fast swap
	void swap(UtlMemory& mem);

	// Switches the buffer from an external memory buffer to a reallocatable buffer
	// Will copy the current contents of the external buffer to the reallocatable buffer
	void convert_to_growable(uint32_t nGrowSize);

	// Size
	[[nodiscard]] uint32_t capacity() const;

	// Grows the memory, so that at least allocated + num elements are allocated
	void grow_by(uint32_t num = 1);

	// Makes sure we've got at least this much memory
	void reserve(uint32_t num);

	// Memory deallocation
	void destroy();

	// destroy all but the given number of elements
	void destroy(uint32_t numElements);

	// is the memory externally allocated?
	[[nodiscard]] bool is_external() const;

	// is the memory read only?
	[[nodiscard]] bool is_read_only() const;

	// Set the capacity by which the memory grows
	void set_grow_size(int capacity);

protected:
	pointer m_Memory;
	uint32_t m_AllocationCount;
	int m_GrowSize;
};


//-----------------------------------------------------------------------------
// The UtlMemory class:
// A growable memory class which doubles in capacity by default.
//-----------------------------------------------------------------------------
template<class _Ty, uint32_t _Size, class _IterTy = uint32_t>
class UtlMemoryFixedGrowable : public UtlMemory< _Ty, _IterTy >
{
public:
	using base_class = UtlMemory<_Ty, _IterTy>;

	UtlMemoryFixedGrowable(uint32_t nGrowSize = 0, uint32_t nInitSize = _Size) : base_class(m_FixedMemory, _Size)
	{
		m_MallocGrowSize = nGrowSize;
	}

	void grow_by(uint32_t nCount = 1)
	{
		if (this->is_external())
		{
			this->convert_to_growable(m_MallocGrowSize);
		}
		base_class::grow_by(nCount);
	}

	void reserve(uint32_t num)
	{
		if (base_class::m_AllocationCount >= num)
			return;

		if (this->is_external())
		{
			// Can'_Ty grow a buffer whose memory was externally allocated 
			this->convert_to_growable(m_MallocGrowSize);
		}

		base_class::reserve(num);
	}

private:
	uint32_t m_MallocGrowSize;
	base_class::value_type m_FixedMemory[_Size];
};

//-----------------------------------------------------------------------------
// The UtlMemoryFixed class:
// A fixed memory class
//-----------------------------------------------------------------------------
template< typename _Ty, uint32_t _Size, uint32_t _Alignment = 0>
class UtlMemoryFixed
{
public:
	using value_type = _Ty;

	using reference = value_type&;
	using const_reference = const value_type&;

	using pointer = value_type*;
	using const_pointer = const value_type*;

	using iterator = pointer;
	using const_iterator = const_pointer;

	// constructor, destructor
	UtlMemoryFixed(uint32_t nGrowSize = 0, uint32_t nInitSize = 0) { }
	UtlMemoryFixed(pointer pMemory, uint32_t numElements) { }

	// Can we use this index?
	// Use unsigned math to improve performance
	[[nodiscard]] bool is_valid_index(uint32_t _IterTy) const { return (uint32_t)_IterTy < _Size; }

	// Specify the invalid ('null') index that we'll only return on failure
	static const uint32_t INVALID_INDEX = -1; // For use with COMPILE_TIME_ASSERT
	static uint32_t invalid_index() { return INVALID_INDEX; }

	// Gets the base address
	[[nodiscard]] pointer data() { if (_Alignment == 0) return (pointer)(&m_Memory[0]); else return (pointer)AlignValue(&m_Memory[0], _Alignment); }
	[[nodiscard]] const_pointer data() const { if (_Alignment == 0) return (pointer)(&m_Memory[0]); else return (pointer)AlignValue(&m_Memory[0], _Alignment); }

	// element access
	// Use unsigned math and inlined checks to improve performance.
	[[nodiscard]] reference operator[](uint32_t _IterTy) { return data()[_IterTy]; }
	[[nodiscard]] const_reference operator[](uint32_t _IterTy) const { return data()[_IterTy]; }
	[[nodiscard]] reference at(uint32_t _IterTy) { return data()[_IterTy]; }
	[[nodiscard]] const_reference at(uint32_t _IterTy) const { return data()[_IterTy]; }

	// Attaches the buffer to external memory....
	void set_external_buffer(pointer pMemory, uint32_t numElements) { }

	// Size
	[[nodiscard]] uint32_t capacity() const { return _Size; }

	// Grows the memory, so that at least allocated + num elements are allocated
	void grow_by(uint32_t num = 1) { }

	// Makes sure we've got at least this much memory
	void reserve(uint32_t num) { }

	// Memory deallocation
	void destroy() { }

	// destroy all but the given number of elements (NOT IMPLEMENTED IN UtlMemoryFixed)
	void destroy(uint32_t numElements) { }

	// is the memory externally allocated?
	[[nodiscard]] bool is_external() const { return false; }

	// Set the capacity by which the memory grows
	void set_grow_size(uint32_t capacity) { }

	class iterator_t
	{
	public:
		iterator_t(uint32_t _IterTy) : index(_IterTy) { }
		uint32_t index;
		bool operator==(const iterator_t it) const { return index == it.index; }
		bool operator!=(const iterator_t it) const { return index != it.index; }
	};
	[[nodiscard]] iterator_t first() const { return iterator_t(is_valid_index(0) ? 0 : invalid_index()); }
	[[nodiscard]] iterator_t next(const iterator_t& it) const { return iterator_t(is_valid_index(it.index + 1) ? it.index + 1 : invalid_index()); }
	[[nodiscard]] uint32_t get_index(const iterator_t& it) const { return it.index; }
	[[nodiscard]] bool is_index_after(uint32_t _IterTy, const iterator_t& it) const { return _IterTy > it.index; }
	[[nodiscard]] bool is_valid_iterator(const iterator_t& it) const { return is_valid_index(it.index); }
	[[nodiscard]] iterator_t invalid_iterator() const { return iterator_t(invalid_index()); }

private:
	char m_Memory[_Size * sizeof(value_type) + _Alignment];
};

#if defined(POSIX)
// From Chris Green: Memory is a little fuzzy but I believe this class did
//	something fishy with respect to msize and alignment that was OK under our
//	allocator, the glibc allocator, etc but not the valgrind one (which has no
//	padding because it detects all forms of head/tail overwrite, including
//	writing 1 byte past a 1 byte allocation).
#define REMEMBER_ALLOC_SIZE_FOR_VALGRIND 1
#endif

//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------

template<class _Ty, class _IterTy>
UtlMemory<_Ty, _IterTy>::UtlMemory(int nGrowSize, uint32_t nInitAllocationCount) : m_Memory(nullptr),
m_AllocationCount(nInitAllocationCount), m_GrowSize(nGrowSize)
{
	if (m_AllocationCount)
		m_Memory = (pointer)malloc(m_AllocationCount * sizeof(value_type));
}

template<class _Ty, class _IterTy>
UtlMemory<_Ty, _IterTy>::UtlMemory(pointer pMemory, uint32_t numElements) : m_Memory(pMemory),
m_AllocationCount(numElements)
{
	// Special marker indicating externally supplied modifyable memory
	m_GrowSize = GrowType_External_Mutable;
}

template<class _Ty, class _IterTy>
UtlMemory<_Ty, _IterTy>::UtlMemory(const_pointer pMemory, uint32_t numElements) : m_Memory((pointer)pMemory),
m_AllocationCount(numElements)
{
	// Special marker indicating externally supplied modifyable memory
	m_GrowSize = GrowType_External_Const;
}

template<class _Ty, class _IterTy>
UtlMemory<_Ty, _IterTy>::~UtlMemory()
{
	destroy();
}

template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::init(int nGrowSize /*= 0*/, uint32_t nInitSize /*= 0*/)
{
	destroy();

	m_GrowSize = nGrowSize;
	m_AllocationCount = nInitSize;
	if (m_AllocationCount)
	{
		m_Memory = static_cast<pointer>(malloc(m_AllocationCount * sizeof(value_type)));
	}
}

//-----------------------------------------------------------------------------
// Fast swap
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::swap(UtlMemory<_Ty, _IterTy>& mem)
{
	std::swap(m_GrowSize, mem.m_GrowSize);
	std::swap(m_Memory, mem.m_Memory);
	std::swap(m_AllocationCount, mem.m_AllocationCount);
}


//-----------------------------------------------------------------------------
// Switches the buffer from an external memory buffer to a reallocatable buffer
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::convert_to_growable(uint32_t nGrowSize)
{
	if (!is_external())
		return;

	m_GrowSize = nGrowSize;
	if (m_AllocationCount)
	{
		uint32_t nNumBytes = m_AllocationCount * sizeof(value_type);
		pointer pMemory = static_cast<pointer>(malloc(nNumBytes));
		std::memcpy(pMemory, m_Memory, nNumBytes);
		m_Memory = pMemory;
	}
	else
	{
		m_Memory = nullptr;
	}
}


//-----------------------------------------------------------------------------
// Attaches the buffer to external memory....
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::set_external_buffer(pointer pMemory, uint32_t numElements)
{
	// Blow away any existing allocated memory
	destroy();

	m_Memory = pMemory;
	m_AllocationCount = numElements;

	// Indicate that we don'_Ty own the memory
	m_GrowSize = GrowType_External_Mutable;
}

template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::set_external_buffer(const_pointer pMemory, uint32_t numElements)
{
	// Blow away any existing allocated memory
	destroy();

	m_Memory = const_cast<pointer>(pMemory);
	m_AllocationCount = numElements;

	// Indicate that we don'_Ty own the memory
	m_GrowSize = GrowType_External_Const;
}

template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::take_ownership(pointer pMemory, uint32_t numElements)
{
	// Blow away any existing allocated memory
	destroy();

	// Simply take the pointer but don'_Ty mark us as external
	m_Memory = pMemory;
	m_AllocationCount = numElements;
}


//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
inline UtlMemory<_Ty, _IterTy>::reference UtlMemory<_Ty, _IterTy>::operator[](_IterTy _IterTy)
{
	return m_Memory[_IterTy];
}

template<class _Ty, class _IterTy>
inline UtlMemory<_Ty, _IterTy>::const_reference UtlMemory<_Ty, _IterTy>::operator[](_IterTy _IterTy) const
{
	return m_Memory[_IterTy];
}

template<class _Ty, class _IterTy>
inline UtlMemory<_Ty, _IterTy>::reference UtlMemory<_Ty, _IterTy>::at(_IterTy _IterTy)
{
	return m_Memory[_IterTy];
}

template<class _Ty, class _IterTy>
inline UtlMemory<_Ty, _IterTy>::const_reference UtlMemory<_Ty, _IterTy>::at(_IterTy _IterTy) const
{
	return m_Memory[_IterTy];
}


//-----------------------------------------------------------------------------
// is the memory externally allocated?
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
bool UtlMemory<_Ty, _IterTy>::is_external() const
{
	return (m_GrowSize < GrowType_Internal);
}


//-----------------------------------------------------------------------------
// is the memory read only?
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
bool UtlMemory<_Ty, _IterTy>::is_read_only() const
{
	return (m_GrowSize == GrowType_External_Const);
}


template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::set_grow_size(int nSize)
{
	m_GrowSize = nSize;
}


//-----------------------------------------------------------------------------
// Gets the base address (can change when adding elements!)
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
inline UtlMemory<_Ty, _IterTy>::pointer UtlMemory<_Ty, _IterTy>::data()
{
	return m_Memory;
}

template<class _Ty, class _IterTy>
inline UtlMemory<_Ty, _IterTy>::const_pointer UtlMemory<_Ty, _IterTy>::data() const
{
	return m_Memory;
}


//-----------------------------------------------------------------------------
// Size
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
inline uint32_t UtlMemory<_Ty, _IterTy>::capacity() const
{
	return m_AllocationCount;
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
inline bool UtlMemory<_Ty, _IterTy>::is_valid_index(_IterTy _IterTy) const
{
	// If we always cast '_IterTy' and 'm_AllocationCount' to unsigned then we can
	// do our range checking with a single comparison instead of two. This gives
	// a modest speedup in debug builds.
	return static_cast<uint32_t>(_IterTy) < m_AllocationCount;
}

template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::grow_by(uint32_t num)
{
	if (is_external())
	{
		// Can'_Ty grow a buffer whose memory was externally allocated 
		return;
	}

	// Make sure we have at least numallocated + num allocations.
	// Use the grow rules specified for this memory (in m_GrowSize)
	uint32_t nAllocationRequested = m_AllocationCount + num;

	auto calc_new_alloc_count = [] (uint32_t nAllocationCount, uint32_t nGrowSize, uint32_t nNewSize, uint32_t nBytesItem)
	{
		if (nGrowSize)
		{
			nAllocationCount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
		}
		else
		{
			if (!nAllocationCount)
			{
				// Compute an allocation which is at least as big as a cache line...
				nAllocationCount = (31 + nBytesItem) / nBytesItem;
			}

			while (nAllocationCount < nNewSize)
				nAllocationCount *= 2;
		}

		return nAllocationCount;
	};
	uint32_t nNewAllocationCount = calc_new_alloc_count(m_AllocationCount, static_cast<uint32_t>(m_GrowSize), nAllocationRequested, sizeof(value_type));

	// if m_nAllocationRequested wraps index type _IterTy, recalculate
	if (static_cast<uint32_t>(nNewAllocationCount) < nAllocationRequested)
	{
		if (static_cast<uint32_t>(nNewAllocationCount) == 0 && static_cast<uint32_t>(nNewAllocationCount - 1) >= nAllocationRequested)
		{
			--nNewAllocationCount; // deal w/ the common case of m_AllocationCount == MAX_USHORT + 1
		}
		else
		{
			if (static_cast<uint32_t>(nAllocationRequested) != nAllocationRequested)
			{
				// we've been asked to grow memory to a capacity s._Ty. the index type can'_Ty address the requested amount of memory
				return;
			}
			while (static_cast<uint32_t>(nNewAllocationCount) < nAllocationRequested)
			{
				nNewAllocationCount = (nNewAllocationCount + nAllocationRequested) / 2;
			}
		}
	}

	if (pointer mem = static_cast<pointer>(std::realloc(m_Memory, nNewAllocationCount * sizeof(value_type))))
	{
		m_Memory = mem;
		m_AllocationCount = nNewAllocationCount;
	}
}


//-----------------------------------------------------------------------------
// Makes sure we've got at least this much memory
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
inline void UtlMemory<_Ty, _IterTy>::reserve(uint32_t num)
{
	if (m_AllocationCount >= num)
		return;

	if (is_external())
	{
		// Can'_Ty grow a buffer whose memory was externally allocated 
		return;
	}

	if (pointer mem = static_cast<pointer>(std::realloc(m_Memory, num * sizeof(value_type))))
	{
		m_Memory = mem;
		m_AllocationCount = num;
	}
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::destroy()
{
	if (!is_external())
	{
		if (m_Memory)
		{
			free(m_Memory);
			m_Memory = nullptr;
		}
		m_AllocationCount = 0;
	}
}

template<class _Ty, class _IterTy>
void UtlMemory<_Ty, _IterTy>::destroy(uint32_t numElements)
{
	if (numElements > m_AllocationCount)
	{
		// Ensure this isn'_Ty a grow request in disguise.
		return;
	}

	// If we have zero elements, simply do a purge:
	if (numElements == 0)
	{
		destroy();
		return;
	}

	if (is_external())
	{
		// Can'_Ty shrink a buffer whose memory was externally allocated, fail silently like purge 
		return;
	}

	// If the number of elements is the same as the allocation count, we are done.
	if (numElements == m_AllocationCount)
	{
		return;
	}


	if (!m_Memory)
	{
		// Allocation count is non zero, but memory is null.
		return;
	}

	if (pointer mem = static_cast<pointer>(std::realloc(m_Memory, numElements * sizeof(value_type))))
	{
		m_Memory = mem;
		m_AllocationCount = numElements;
	}
}

TF2_NAMESPACE_END();
