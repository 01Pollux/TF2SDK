#pragma once

#include "TF2Config.hpp"
#include "UtlMemory.hpp"

TF2_NAMESPACE_BEGIN(::Utils);

// UtlVector derives from this so we can do the type check above
struct UtlBaseVector
{
public:
	static constexpr bool IsUtlVector = true;
};


//-----------------------------------------------------------------------------
// The UtlVector class:
// _AllocTy growable array class which doubles in size by default.
// It will always keep all elements consecutive in memory, and may move the
// elements around in memory (via _AllocTy PvRealloc) when elements are inserted or
// removed. Clients should therefore refer to the elements of the vector
// by index (they should *never* maintain pointers to elements in the vector).
//-----------------------------------------------------------------------------
template<class _Ty, class _AllocTy = UtlMemory<_Ty>>
class UtlVector : public UtlBaseVector
{
	using CAllocator = _AllocTy;
public:
	using ElemType_t = _Ty;
	using iterator = _Ty*;
	using const_iterator = const _Ty*;

	// Set the growth policy and initial capacity. size will always be zero. This is different from std::vector
	// where the constructor sets count as well as capacity.
	// growSize of zero implies the default growth pattern which is exponential.
	explicit UtlVector(int growSize = 0, uint32_t initialCapacity = 0);

	// Initialize with separately allocated buffer, setting the capacity and count.
	// The container will not be growable.
	UtlVector(_Ty* pMemory, uint32_t initialCapacity, uint32_t initialCount = 0);
	~UtlVector();

	// Copy the array.
	UtlVector& operator=(const UtlVector& other);
	UtlVector(const UtlVector&) = delete;

	// element access
	_Ty& operator[](uint32_t i);
	const _Ty& operator[](uint32_t i) const;
	_Ty& at(uint32_t i);
	const _Ty& at(uint32_t i) const;
	_Ty& head();
	const _Ty& head() const;
	_Ty& tail();
	const _Ty& tail() const;

	// STL compatible member functions. These allow easier use of std::sort
	// and they are forward compatible with the C++ 11 range-based for loops.
	iterator begin() { return data(); }
	const_iterator begin() const { return data(); }
	iterator end() { return data() + size(); }
	const_iterator end() const { return data() + size(); }

	// Gets the base address (can change when adding elements!)
	_Ty* data() { return m_Memory.data(); }
	const _Ty* data() const { return m_Memory.data(); }

	// Returns the number of elements in the vector
	uint32_t size() const;

	/// are there no elements? For compatibility with lists.
	inline bool is_empty() const
	{
		return (size() == 0);
	}

	// Is element index valid?
	bool is_valid_index(uint32_t i) const;
	static uint32_t invalid_index();

	// Adds an element, uses default constructor
	uint32_t push_to_head();
	uint32_t push_to_tail();
	_Ty* push_to_tail_ptr();
	uint32_t push_before(uint32_t elem);
	uint32_t push_after(uint32_t elem);

	// Adds an element, uses copy constructor
	uint32_t push_to_head(const _Ty& src);
	uint32_t push_to_tail(const _Ty& src);
	uint32_t push_before(uint32_t elem, const _Ty& src);
	uint32_t push_after(uint32_t elem, const _Ty& src);

	// Adds multiple elements, uses default constructor
	uint32_t push_to_head_multiple(uint32_t num);
	uint32_t push_to_tail_multiple(uint32_t num);
	uint32_t push_to_tail_multiple(uint32_t num, const _Ty* pToCopy);
	uint32_t push_before_multiple(uint32_t elem, uint32_t num);
	uint32_t push_before_multiple(uint32_t elem, uint32_t num, const _Ty* pToCopy);
	uint32_t push_after_multiple(uint32_t elem, uint32_t num);

	// resize deletes the previous contents of the container and sets the
	// container to have this many elements.
	// Calls clear() then push_to_tail_multiple.
	// Use GetCount to retrieve the current count.
	void resize(uint32_t count);
	void resize_no_destroy(uint32_t count); //sets count by adding or removing elements to tail TODO: This should probably be the default behavior for resize

	// Calls resize and copies each element.
	void copy(const _Ty* pArray, uint32_t size);

	// Fast swap
	void swap(UtlVector< _Ty, _AllocTy >& vec);

	// Add the specified array to the tail.
	uint32_t push_to_tail(const UtlVector& src);

	// Finds an element (element needs operator== defined)
	uint32_t Find(const _Ty& src) const;

	// Helper to find using std::find_if with _AllocTy predicate
	//   e.g. [] -> bool ( _Ty &_AllocTy ) { return _AllocTy.IsTheThingIWant(); }
	//
	// Useful if your object doesn'_Ty define _AllocTy ==
	template<typename _FnTy>
	uint32_t find(_FnTy predicate) const;

	void fill(const _Ty& src);

	bool contains(const _Ty& src) const;

	// Makes sure we have enough memory allocated to store _AllocTy requested # of elements
	// Use capacity() to retrieve the current capacity.
	void reserve(uint32_t num);

	// Makes sure we have at least this many elements
	// Use GetCount to retrieve the current count.
	void reserve_size(uint32_t num);

	// at removal
	void erase_fast(uint32_t elem);	// doesn'_Ty preserve order
	void erase(uint32_t elem);		// preserves order, shifts elements
	bool find_and_erase(const _Ty& src);	// removes first occurrence of src, preserves order, shifts elements
	bool find_and_erase_fast(const _Ty& src);	// removes first occurrence of src, doesn'_Ty preserve order
	void erase_multiple(uint32_t elem, uint32_t num);	// preserves order, shifts elements
	void erase_from_head_multiple(uint32_t num); // removes num elements from tail
	void erase_from_tail_multiple(uint32_t num); // removes num elements from tail
	void clear();				// doesn'_Ty deallocate memory

	// Memory deallocation
	void destroy();

	// Purges the list and calls delete on each element in it.
	void destroy_and_delete();

	// Compacts the vector to the number of elements actually in use 
	void destroy_no_resize();

	// Set the size by which it grows when it needs to allocate more memory.
	void set_grow_size(int size) { m_Memory.set_grow_size(size); }

	uint32_t capacity() const;	// Only use this if you really know what you're doing!

	// reverse the order of elements
	void reverse();

protected:
	// Grows the vector
	void grow_by(uint32_t num = 1);

	// Shifts elements....
	void shift_to_right(uint32_t elem, uint32_t num = 1);
	void shift_to_left(uint32_t elem, uint32_t num = 1);

	CAllocator m_Memory;
	uint32_t m_Size;

	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	_Ty* m_Elements;

	inline void reset_dbg_info()
	{
		m_Elements = data();
	}
};


//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
inline UtlVector<_Ty, _AllocTy>::UtlVector(int growSize, uint32_t initSize) :
	m_Memory(growSize, initSize), m_Size(0)
{
	reset_dbg_info();
}

template<typename _Ty, class _AllocTy>
inline UtlVector<_Ty, _AllocTy>::UtlVector(_Ty* pMemory, uint32_t allocationCount, uint32_t numElements) :
	m_Memory(pMemory, allocationCount), m_Size(numElements)
{
	reset_dbg_info();
}

template<typename _Ty, class _AllocTy>
inline UtlVector<_Ty, _AllocTy>::~UtlVector()
{
	destroy();
}

template<typename _Ty, class _AllocTy>
inline UtlVector<_Ty, _AllocTy>& UtlVector<_Ty, _AllocTy>::operator=(const UtlVector<_Ty, _AllocTy>& other)
{
	uint32_t nCount = other.size();
	resize(nCount);
	for (uint32_t i = 0; i < nCount; i++)
	{
		(*this)[i] = other[i];
	}
	return *this;
}

//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
inline _Ty& UtlVector<_Ty, _AllocTy>::operator[](uint32_t i)
{
	return m_Memory[i];
}

template<typename _Ty, class _AllocTy>
inline const _Ty& UtlVector<_Ty, _AllocTy>::operator[](uint32_t i) const
{
	return m_Memory[i];
}

template<typename _Ty, class _AllocTy>
inline _Ty& UtlVector<_Ty, _AllocTy>::at(uint32_t i)
{
	return m_Memory[i];
}

template<typename _Ty, class _AllocTy>
inline const _Ty& UtlVector<_Ty, _AllocTy>::at(uint32_t i) const
{
	return m_Memory[i];
}

template<typename _Ty, class _AllocTy>
inline _Ty& UtlVector<_Ty, _AllocTy>::head()
{
	return m_Memory[0];
}

template<typename _Ty, class _AllocTy>
inline const _Ty& UtlVector<_Ty, _AllocTy>::head() const
{
	return m_Memory[0];
}

template<typename _Ty, class _AllocTy>
inline _Ty& UtlVector<_Ty, _AllocTy>::tail()
{
	return m_Memory[m_Size - 1];
}

template<typename _Ty, class _AllocTy>
inline const _Ty& UtlVector<_Ty, _AllocTy>::tail() const
{
	return m_Memory[m_Size - 1];
}



template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::size() const
{
	return m_Size;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// reverse - reverse the order of elements, akin to std::vector<>::reverse()
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::reverse()
{
	for (uint32_t i = 0; i < m_Size / 2; i++)
	{
		std::swap(m_Memory[i], m_Memory[m_Size - 1 - i]);
	}
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
inline bool UtlVector<_Ty, _AllocTy>::is_valid_index(uint32_t i) const
{
	return (i >= 0) && (i < m_Size);
}


//-----------------------------------------------------------------------------
// Returns in invalid index
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::invalid_index()
{
	return static_cast<uint32_t>(-1);
}


//-----------------------------------------------------------------------------
// Grows the vector
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::grow_by(uint32_t num)
{
	if (m_Size + num > m_Memory.capacity())
	{
		m_Memory.grow_by(m_Size + num - m_Memory.capacity());
	}

	m_Size += num;
	reset_dbg_info();
}


//-----------------------------------------------------------------------------
// Makes sure we have enough memory allocated to store _AllocTy requested # of elements
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::reserve(uint32_t num)
{
	m_Memory.reserve(num);
	reset_dbg_info();
}


//-----------------------------------------------------------------------------
// Makes sure we have at least this many elements
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::reserve_size(uint32_t num)
{
	if (size() < num)
	{
		push_to_tail_multiple(num - size());
	}
}


//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::shift_to_right(uint32_t elem, uint32_t num)
{
	uint32_t numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
		memmove(&at(elem + num), &at(elem), numToMove * sizeof(_Ty));
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::shift_to_left(uint32_t elem, uint32_t num)
{
	uint32_t numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
	{
		memmove(&at(elem), &at(elem + num), numToMove * sizeof(_Ty));
	}
}


//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_to_head()
{
	return push_before(0);
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_to_tail()
{
	return push_before(m_Size);
}

template<typename _Ty, class _AllocTy>
inline _Ty* UtlVector<_Ty, _AllocTy>::push_to_tail_ptr()
{
	return &at(push_to_tail());
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_after(uint32_t elem)
{
	return push_before(elem + 1);
}

template<typename _Ty, class _AllocTy>
uint32_t UtlVector<_Ty, _AllocTy>::push_before(uint32_t elem)
{
	grow_by();
	shift_to_right(elem);
	Utils::VAlloc::Construct(&at(elem));
	return elem;
}


//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_to_head(const _Ty& src)
{
	return push_before(0, src);
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_to_tail(const _Ty& src)
{
	return push_before(m_Size, src);
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_after(uint32_t elem, const _Ty& src)
{
	return push_before(elem + 1, src);
}

template<typename _Ty, class _AllocTy>
uint32_t UtlVector<_Ty, _AllocTy>::push_before(uint32_t elem, const _Ty& src)
{
	grow_by();
	shift_to_right(elem);
	Utils::VAlloc::CopyConstruct(&at(elem), src);
	return elem;
}


//-----------------------------------------------------------------------------
// Adds multiple elements, uses default constructor
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_to_head_multiple(uint32_t num)
{
	return push_before_multiple(0, num);
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_to_tail_multiple(uint32_t num)
{
	return push_before_multiple(m_Size, num);
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_to_tail_multiple(uint32_t num, const _Ty* pToCopy)
{
	return push_before_multiple(m_Size, num, pToCopy);
}

template<typename _Ty, class _AllocTy>
uint32_t UtlVector<_Ty, _AllocTy>::push_after_multiple(uint32_t elem, uint32_t num)
{
	return push_before_multiple(elem + 1, num);
}


template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::resize(uint32_t count)
{
	clear();
	push_to_tail_multiple(count);
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::resize_no_destroy(uint32_t count)
{
	uint32_t delta = count - m_Size;
	if (delta > 0) push_to_tail_multiple(delta);
	else if (delta < 0) erase_from_tail_multiple(-delta);
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::copy(const _Ty* pArray, uint32_t size)
{
	// Can'_Ty insert something that's in the list... reallocation may hose us
	resize(size);
	for (uint32_t i = 0; i < size; i++)
	{
		(*this)[i] = pArray[i];
	}
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::swap(UtlVector< _Ty, _AllocTy >& vec)
{
	m_Memory.swap(vec.m_Memory);
	std::swap(m_Size, vec.m_Size);
	std::swap(m_Elements, vec.m_Elements);
}

template<typename _Ty, class _AllocTy>
uint32_t UtlVector<_Ty, _AllocTy>::push_to_tail(const UtlVector& src)
{
	uint32_t base = size();

	// Make space.
	uint32_t nSrcCount = src.size();
	reserve(base + nSrcCount);

	// Copy the elements.	
	m_Size += nSrcCount;
	for (uint32_t i = 0; i < nSrcCount; i++)
	{
		Utils::VAlloc::CopyConstruct(&at(base + i), src[i]);
	}
	return base;
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_before_multiple(uint32_t elem, uint32_t num)
{
	if (num == 0)
		return elem;

	grow_by(num);
	shift_to_right(elem, num);

	// Invoke default constructors
	for (uint32_t i = 0; i < num; ++i)
	{
		Utils::VAlloc::Construct(&at(elem + i));
	}

	return elem;
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::push_before_multiple(uint32_t elem, uint32_t num, const _Ty* pToInsert)
{
	if (num == 0)
		return elem;

	grow_by(num);
	shift_to_right(elem, num);

	// Invoke default constructors
	if (!pToInsert)
	{
		for (uint32_t i = 0; i < num; ++i)
		{
			Utils::VAlloc::Construct(&at(elem + i));
		}
	}
	else
	{
		for (uint32_t i = 0; i < num; i++)
		{
			Utils::VAlloc::CopyConstruct(&at(elem + i), pToInsert[i]);
		}
	}

	return elem;
}


//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
uint32_t UtlVector<_Ty, _AllocTy>::Find(const _Ty& src) const
{
	for (uint32_t i = 0; i < size(); ++i)
	{
		if (at(i) == src)
			return i;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Finds an element using _AllocTy predicate, using std::find_if
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
template<typename _FnTy>
uint32_t UtlVector<_Ty, _AllocTy>::find(_FnTy predicate) const
{
	const _Ty* begin = data();
	const _Ty* end = begin + size();
	const _Ty* const& elem = std::find_if(begin, end, predicate);

	if (elem != end)
	{
		return std::distance(begin, elem);
	}

	return invalid_index();
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::fill(const _Ty& src)
{
	for (uint32_t i = 0; i < size(); i++)
	{
		at(i) = src;
	}
}

template<typename _Ty, class _AllocTy>
bool UtlVector<_Ty, _AllocTy>::contains(const _Ty& src) const
{
	return (Find(src) >= 0);
}


//-----------------------------------------------------------------------------
// at removal
//-----------------------------------------------------------------------------
template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::erase_fast(uint32_t elem)
{
	// Global scope to resolve conflict with Scaleform 4.0
	VAlloc::Destruct(&at(elem));
	if (m_Size > 0)
	{
		if (elem != m_Size - 1)
			memcpy(&at(elem), &at(m_Size - 1), sizeof(_Ty));
		--m_Size;
	}
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::erase(uint32_t elem)
{
	// Global scope to resolve conflict with Scaleform 4.0
	VAlloc::Destruct(&at(elem));
	shift_to_left(elem);
	--m_Size;
}

template<typename _Ty, class _AllocTy>
bool UtlVector<_Ty, _AllocTy>::find_and_erase(const _Ty& src)
{
	uint32_t elem = Find(src);
	if (elem != -1)
	{
		erase(elem);
		return true;
	}
	return false;
}

template<typename _Ty, class _AllocTy>
bool UtlVector<_Ty, _AllocTy>::find_and_erase_fast(const _Ty& src)
{
	uint32_t elem = Find(src);
	if (elem != -1)
	{
		erase_fast(elem);
		return true;
	}
	return false;
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::erase_multiple(uint32_t elem, uint32_t num)
{
	// Global scope to resolve conflict with Scaleform 4.0
	for (uint32_t i = elem + num; --i >= elem; )
		VAlloc::Destruct(&at(i));

	shift_to_left(elem, num);
	m_Size -= num;
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::erase_from_head_multiple(uint32_t num)
{
	// Global scope to resolve conflict with Scaleform 4.0
	for (uint32_t i = num; --i >= 0; )
		VAlloc::Destruct(&at(i));

	shift_to_left(0, num);
	m_Size -= num;
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::erase_from_tail_multiple(uint32_t num)
{
	// Global scope to resolve conflict with Scaleform 4.0
	for (uint32_t i = m_Size - num; i < m_Size; i++)
		VAlloc::Destruct(&at(i));

	m_Size -= num;
}

template<typename _Ty, class _AllocTy>
void UtlVector<_Ty, _AllocTy>::clear()
{
	for (uint32_t i = m_Size; --i >= 0; )
	{
		// Global scope to resolve conflict with Scaleform 4.0
		VAlloc::Destruct(&at(i));
	}

	m_Size = 0;
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------

template<typename _Ty, class _AllocTy>
inline void UtlVector<_Ty, _AllocTy>::destroy()
{
	clear();
	m_Memory.destroy();
	reset_dbg_info();
}


template<typename _Ty, class _AllocTy>
inline void UtlVector<_Ty, _AllocTy>::destroy_and_delete()
{
	for (uint32_t i = 0; i < m_Size; i++)
	{
		delete at(i);
	}
	destroy();
}

template<typename _Ty, class _AllocTy>
inline void UtlVector<_Ty, _AllocTy>::destroy_no_resize()
{
	m_Memory.destroy(m_Size);
}

template<typename _Ty, class _AllocTy>
inline uint32_t UtlVector<_Ty, _AllocTy>::capacity() const
{
	return m_Memory.capacity();
}



TF2_NAMESPACE_END();
