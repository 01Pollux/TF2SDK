#pragma once

#include "Config.hpp"
#include "UtlAlloc.hpp"
#include <memory>


TF2_NAMESPACE_BEGIN(::Utils);

//-----------------------------------------------------------------------------
// The UtlMemory class:
// A growable memory class which doubles in size by default.
//-----------------------------------------------------------------------------
template<class T, typename I = int>
class UtlMemory
{
public:
	// constructor, destructor
	UtlMemory(int nGrowSize = 0, int nInitSize = 0);
	UtlMemory(T* pMemory, int numElements);
	UtlMemory(const T* pMemory, int numElements);
	~UtlMemory();

	// Set the size by which the memory grows
	void Init(int nGrowSize = 0, int nInitSize = 0);

	class Iterator_t
	{
	public:
		Iterator_t(I i) : index(i) { }
		I index;

		bool operator==(const Iterator_t it) const { return index == it.index; }
		bool operator!=(const Iterator_t it) const { return index != it.index; }
	};
	Iterator_t First() const { return Iterator_t(IsIdxValid(0) ? 0 : InvalidIndex()); }
	Iterator_t Next(const Iterator_t& it) const { return Iterator_t(IsIdxValid(it.index + 1) ? it.index + 1 : InvalidIndex()); }
	I GetIndex(const Iterator_t& it) const { return it.index; }
	bool IsIdxAfter(I i, const Iterator_t& it) const { return i > it.index; }
	bool IsValidIterator(const Iterator_t& it) const { return IsIdxValid(it.index); }
	Iterator_t InvalidIterator() const { return Iterator_t(InvalidIndex()); }

	// element access
	T& operator[](I i);
	const T& operator[](I i) const;
	T& Element(I i);
	const T& Element(I i) const;

	// Can we use this index?
	bool IsIdxValid(I i) const;

	// Specify the invalid ('null') index that we'll only return on failure
	static const I INVALID_INDEX = (I)-1; // For use with COMPILE_TIME_ASSERT
	static I InvalidIndex() { return INVALID_INDEX; }

	// Gets the base address (can change when adding elements!)
	T* Base();
	const T* Base() const;

	// Attaches the buffer to external memory....
	void SetExternalBuffer(T* pMemory, int numElements);
	void SetExternalBuffer(const T* pMemory, int numElements);
	// Takes ownership of the passed memory, including freeing it when this buffer is destroyed.
	void AssumeMemory(T* pMemory, int nSize);

	// Fast swap
	void Swap(UtlMemory< T, I >& mem);

	// Switches the buffer from an external memory buffer to a reallocatable buffer
	// Will copy the current contents of the external buffer to the reallocatable buffer
	void ConvertToGrowableMemory(int nGrowSize);

	// Size
	int NumAllocated() const;
	int Count() const;

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow(int num = 1);

	// Makes sure we've got at least this much memory
	void EnsureCapacity(int num);

	// Memory deallocation
	void Purge();

	// Purge all but the given number of elements
	void Purge(int numElements);

	// is the memory externally allocated?
	bool IsExternallyAllocated() const;

	// is the memory read only?
	bool IsReadOnly() const;

	// Set the size by which the memory grows
	void SetGrowSize(int size);

protected:
	enum class GrowType : int
	{
		External_Const = -2,
		External_Mutable = -1,
		Internal = 0,
	};

	T* m_Memory;
	size_t m_AllocationCount;
	GrowType m_GrowSize;
};


//-----------------------------------------------------------------------------
// The UtlMemory class:
// A growable memory class which doubles in size by default.
//-----------------------------------------------------------------------------
template<class T, size_t SIZE, class I = int>
class UtlMemoryFixedGrowable : public UtlMemory< T, I >
{
	typedef UtlMemory< T, I > BaseClass;

public:
	UtlMemoryFixedGrowable(int nGrowSize = 0, int nInitSize = SIZE) : BaseClass(m_FixedMemory, SIZE)
	{
		m_MallocGrowSize = nGrowSize;
	}

	void Grow(int nCount = 1)
	{
		if (this->IsExternallyAllocated())
		{
			this->ConvertToGrowableMemory(m_MallocGrowSize);
		}
		BaseClass::Grow(nCount);
	}

	void EnsureCapacity(int num)
	{
		if (UtlMemory<T>::m_AllocationCount >= num)
			return;

		if (this->IsExternallyAllocated())
		{
			// Can't grow a buffer whose memory was externally allocated 
			this->ConvertToGrowableMemory(m_MallocGrowSize);
		}

		BaseClass::EnsureCapacity(num);
	}

private:
	int m_MallocGrowSize;
	T m_FixedMemory[SIZE];
};

//-----------------------------------------------------------------------------
// The UtlMemoryFixed class:
// A fixed memory class
//-----------------------------------------------------------------------------
template< typename T, size_t SIZE, int nAlignment = 0>
class UtlMemoryFixed
{
public:
	// constructor, destructor
	UtlMemoryFixed(int nGrowSize = 0, int nInitSize = 0) { }
	UtlMemoryFixed(T* pMemory, int numElements) { }

	// Can we use this index?
	// Use unsigned math to improve performance
	bool IsIdxValid(int i) const { return (size_t)i < SIZE; }

	// Specify the invalid ('null') index that we'll only return on failure
	static const int INVALID_INDEX = -1; // For use with COMPILE_TIME_ASSERT
	static int InvalidIndex() { return INVALID_INDEX; }

	// Gets the base address
	T* Base() { if (nAlignment == 0) return (T*)(&m_Memory[0]); else return (T*)AlignValue(&m_Memory[0], nAlignment); }
	const T* Base() const { if (nAlignment == 0) return (T*)(&m_Memory[0]); else return (T*)AlignValue(&m_Memory[0], nAlignment); }

	// element access
	// Use unsigned math and inlined checks to improve performance.
	T& operator[](int i) { return Base()[i]; }
	const T& operator[](int i) const { return Base()[i]; }
	T& Element(int i) { return Base()[i]; }
	const T& Element(int i) const { return Base()[i]; }

	// Attaches the buffer to external memory....
	void SetExternalBuffer(T* pMemory, int numElements) { }

	// Size
	int NumAllocated() const { return SIZE; }
	int Count() const { return SIZE; }

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow(int num = 1) { }

	// Makes sure we've got at least this much memory
	void EnsureCapacity(int num) { }

	// Memory deallocation
	void Purge() { }

	// Purge all but the given number of elements (NOT IMPLEMENTED IN UtlMemoryFixed)
	void Purge(int numElements) { }

	// is the memory externally allocated?
	bool IsExternallyAllocated() const { return false; }

	// Set the size by which the memory grows
	void SetGrowSize(int size) { }

	class Iterator_t
	{
	public:
		Iterator_t(int i) : index(i) { }
		int index;
		bool operator==(const Iterator_t it) const { return index == it.index; }
		bool operator!=(const Iterator_t it) const { return index != it.index; }
	};
	Iterator_t First() const { return Iterator_t(IsIdxValid(0) ? 0 : InvalidIndex()); }
	Iterator_t Next(const Iterator_t& it) const { return Iterator_t(IsIdxValid(it.index + 1) ? it.index + 1 : InvalidIndex()); }
	int GetIndex(const Iterator_t& it) const { return it.index; }
	bool IsIdxAfter(int i, const Iterator_t& it) const { return i > it.index; }
	bool IsValidIterator(const Iterator_t& it) const { return IsIdxValid(it.index); }
	Iterator_t InvalidIterator() const { return Iterator_t(InvalidIndex()); }

private:
	char m_Memory[SIZE * sizeof(T) + nAlignment];
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

template<class T, class I>
UtlMemory<T, I>::UtlMemory(int nGrowSize, int nInitAllocationCount) : m_Memory(0),
m_AllocationCount(nInitAllocationCount), m_GrowSize(static_cast<GrowType>(nGrowSize))
{
	if (m_AllocationCount)
		m_Memory = (T*)malloc(m_AllocationCount * sizeof(T));
}

template<class T, class I>
UtlMemory<T, I>::UtlMemory(T* pMemory, int numElements) : m_Memory(pMemory),
m_AllocationCount(numElements)
{
	// Special marker indicating externally supplied modifyable memory
	m_GrowSize = GrowType::External_Mutable;
}

template<class T, class I>
UtlMemory<T, I>::UtlMemory(const T* pMemory, int numElements) : m_Memory((T*)pMemory),
m_AllocationCount(numElements)
{
	// Special marker indicating externally supplied modifyable memory
	m_GrowSize = GrowType::External_Const;
}

template<class T, class I>
UtlMemory<T, I>::~UtlMemory()
{
	Purge();
}

template<class T, class I>
void UtlMemory<T, I>::Init(int nGrowSize /*= 0*/, int nInitSize /*= 0*/)
{
	Purge();

	m_GrowSize = nGrowSize;
	m_AllocationCount = nInitSize;
	if (m_AllocationCount)
	{
		m_Memory = (T*)malloc(m_AllocationCount * sizeof(T));
	}
}

//-----------------------------------------------------------------------------
// Fast swap
//-----------------------------------------------------------------------------
template<class T, class I>
void UtlMemory<T, I>::Swap(UtlMemory<T, I>& mem)
{
	std::swap(m_GrowSize, mem.m_GrowSize);
	std::swap(m_Memory, mem.m_Memory);
	std::swap(m_AllocationCount, mem.m_AllocationCount);
}


//-----------------------------------------------------------------------------
// Switches the buffer from an external memory buffer to a reallocatable buffer
//-----------------------------------------------------------------------------
template<class T, class I>
void UtlMemory<T, I>::ConvertToGrowableMemory(int nGrowSize)
{
	if (!IsExternallyAllocated())
		return;

	m_GrowSize = nGrowSize;
	if (m_AllocationCount)
	{
		int nNumBytes = m_AllocationCount * sizeof(T);
		T* pMemory = (T*)malloc(nNumBytes);
		memcpy((void*)pMemory, (void*)m_Memory, nNumBytes);
		m_Memory = pMemory;
	}
	else
	{
		m_Memory = NULL;
	}
}


//-----------------------------------------------------------------------------
// Attaches the buffer to external memory....
//-----------------------------------------------------------------------------
template<class T, class I>
void UtlMemory<T, I>::SetExternalBuffer(T* pMemory, int numElements)
{
	// Blow away any existing allocated memory
	Purge();

	m_Memory = pMemory;
	m_AllocationCount = numElements;

	// Indicate that we don't own the memory
	m_GrowSize = GrowType::External_Mutable;
}

template<class T, class I>
void UtlMemory<T, I>::SetExternalBuffer(const T* pMemory, int numElements)
{
	// Blow away any existing allocated memory
	Purge();

	m_Memory = const_cast<T*>(pMemory);
	m_AllocationCount = numElements;

	// Indicate that we don't own the memory
	m_GrowSize = GrowType::External_Const;
}

template<class T, class I>
void UtlMemory<T, I>::AssumeMemory(T* pMemory, int numElements)
{
	// Blow away any existing allocated memory
	Purge();

	// Simply take the pointer but don't mark us as external
	m_Memory = pMemory;
	m_AllocationCount = numElements;
}


//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template<class T, class I>
inline T& UtlMemory<T, I>::operator[](I i)
{
	return m_Memory[(uint32_t)i];
}

template<class T, class I>
inline const T& UtlMemory<T, I>::operator[](I i) const
{
	return m_Memory[(uint32_t)i];
}

template<class T, class I>
inline T& UtlMemory<T, I>::Element(I i)
{
	return m_Memory[(uint32_t)i];
}

template<class T, class I>
inline const T& UtlMemory<T, I>::Element(I i) const
{
	return m_Memory[(uint32_t)i];
}


//-----------------------------------------------------------------------------
// is the memory externally allocated?
//-----------------------------------------------------------------------------
template<class T, class I>
bool UtlMemory<T, I>::IsExternallyAllocated() const
{
	return (m_GrowSize < GrowType::Internal);
}


//-----------------------------------------------------------------------------
// is the memory read only?
//-----------------------------------------------------------------------------
template<class T, class I>
bool UtlMemory<T, I>::IsReadOnly() const
{
	return (m_GrowSize == GrowType::External_Const);
}


template<class T, class I>
void UtlMemory<T, I>::SetGrowSize(int nSize)
{
	m_GrowSize = nSize;
}


//-----------------------------------------------------------------------------
// Gets the base address (can change when adding elements!)
//-----------------------------------------------------------------------------
template<class T, class I>
inline T* UtlMemory<T, I>::Base()
{
	return m_Memory;
}

template<class T, class I>
inline const T* UtlMemory<T, I>::Base() const
{
	return m_Memory;
}


//-----------------------------------------------------------------------------
// Size
//-----------------------------------------------------------------------------
template<class T, class I>
inline int UtlMemory<T, I>::NumAllocated() const
{
	return m_AllocationCount;
}

template<class T, class I>
inline int UtlMemory<T, I>::Count() const
{
	return m_AllocationCount;
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template<class T, class I>
inline bool UtlMemory<T, I>::IsIdxValid(I i) const
{
	// If we always cast 'i' and 'm_AllocationCount' to unsigned then we can
	// do our range checking with a single comparison instead of two. This gives
	// a modest speedup in debug builds.
	return (uint32_t)i < m_AllocationCount;
}

template<class T, class I>
void UtlMemory<T, I>::Grow(int num)
{
	if (IsExternallyAllocated())
	{
		// Can't grow a buffer whose memory was externally allocated 
		return;
	}

	// Make sure we have at least numallocated + num allocations.
	// Use the grow rules specified for this memory (in m_GrowSize)
	int nAllocationRequested = m_AllocationCount + num;

	auto calc_new_alloc_count = [] (int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem)
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
	int nNewAllocationCount = calc_new_alloc_count(m_AllocationCount, static_cast<int>(m_GrowSize), nAllocationRequested, sizeof(T));

	// if m_nAllocationRequested wraps index type I, recalculate
	if ((int)(I)nNewAllocationCount < nAllocationRequested)
	{
		if ((int)(I)nNewAllocationCount == 0 && (int)(I)(nNewAllocationCount - 1) >= nAllocationRequested)
		{
			--nNewAllocationCount; // deal w/ the common case of m_AllocationCount == MAX_USHORT + 1
		}
		else
		{
			if ((int)(I)nAllocationRequested != nAllocationRequested)
			{
				// we've been asked to grow memory to a size s.t. the index type can't address the requested amount of memory
				return;
			}
			while ((int)(I)nNewAllocationCount < nAllocationRequested)
			{
				nNewAllocationCount = (nNewAllocationCount + nAllocationRequested) / 2;
			}
		}
	}

	if (T* mem = (T*)realloc(m_Memory, nNewAllocationCount * sizeof(T)))
	{
		m_Memory = mem;
		m_AllocationCount = nNewAllocationCount;
	}
}


//-----------------------------------------------------------------------------
// Makes sure we've got at least this much memory
//-----------------------------------------------------------------------------
template<class T, class I>
inline void UtlMemory<T, I>::EnsureCapacity(int num)
{
	if (m_AllocationCount >= num)
		return;

	if (IsExternallyAllocated())
	{
		// Can't grow a buffer whose memory was externally allocated 
		return;
	}

	if (T* mem = realloc(m_Memory, num * sizeof(T)))
	{
		m_Memory = mem;
		m_AllocationCount = num;
	}
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template<class T, class I>
void UtlMemory<T, I>::Purge()
{
	if (!IsExternallyAllocated())
	{
		if (m_Memory)
		{
			free(m_Memory);
			m_Memory = nullptr;
		}
		m_AllocationCount = 0;
	}
}

template<class T, class I>
void UtlMemory<T, I>::Purge(int numElements)
{
	if (numElements > m_AllocationCount)
	{
		// Ensure this isn't a grow request in disguise.
		return;
	}

	// If we have zero elements, simply do a purge:
	if (numElements == 0)
	{
		Purge();
		return;
	}

	if (IsExternallyAllocated())
	{
		// Can't shrink a buffer whose memory was externally allocated, fail silently like purge 
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

	if (T* mem = realloc(m_Memory, numElements * sizeof(T)))
	{
		m_Memory = mem;
		m_AllocationCount = numElements;
	}
}

TF2_NAMESPACE_END();
