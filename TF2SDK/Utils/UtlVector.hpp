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
// A growable array class which doubles in size by default.
// It will always keep all elements consecutive in memory, and may move the
// elements around in memory (via a PvRealloc) when elements are inserted or
// removed. Clients should therefore refer to the elements of the vector
// by index (they should *never* maintain pointers to elements in the vector).
//-----------------------------------------------------------------------------
template<class T, class A = UtlMemory<T>>
class UtlVector : public UtlBaseVector
{
	using CAllocator = A;
public:
	using ElemType_t = T;
	using iterator = T*;
	using const_iterator = const T*;

	// Set the growth policy and initial capacity. Count will always be zero. This is different from std::vector
	// where the constructor sets count as well as capacity.
	// growSize of zero implies the default growth pattern which is exponential.
	explicit UtlVector(int growSize = 0, int initialCapacity = 0);

	// Initialize with separately allocated buffer, setting the capacity and count.
	// The container will not be growable.
	UtlVector(T* pMemory, int initialCapacity, int initialCount = 0);
	~UtlVector();

	// Copy the array.
	UtlVector<T, A>& operator=(const UtlVector<T, A>& other);

	// element access
	T& operator[](int i);
	const T& operator[](int i) const;
	T& Element(int i);
	const T& Element(int i) const;
	T& Head();
	const T& Head() const;
	T& Tail();
	const T& Tail() const;

	// STL compatible member functions. These allow easier use of std::sort
	// and they are forward compatible with the C++ 11 range-based for loops.
	iterator begin() { return Base(); }
	const_iterator begin() const { return Base(); }
	iterator end() { return Base() + Count(); }
	const_iterator end() const { return Base() + Count(); }

	// Gets the base address (can change when adding elements!)
	T* Base() { return m_Memory.Base(); }
	const T* Base() const { return m_Memory.Base(); }

	// Returns the number of elements in the vector
	// SIZE IS DEPRECATED!
	int Count() const;

	/// are there no elements? For compatibility with lists.
	inline bool IsEmpty() const
	{
		return (Count() == 0);
	}

	// Is element index valid?
	bool IsValidIndex(int i) const;
	static int InvalidIndex();

	// Adds an element, uses default constructor
	int AddToHead();
	int AddToTail();
	T* AddToTailGetPtr();
	int InsertBefore(int elem);
	int InsertAfter(int elem);

	// Adds an element, uses copy constructor
	int AddToHead(const T& src);
	int AddToTail(const T& src);
	int InsertBefore(int elem, const T& src);
	int InsertAfter(int elem, const T& src);

	// Adds multiple elements, uses default constructor
	int AddMultipleToHead(int num);
	int AddMultipleToTail(int num);
	int AddMultipleToTail(int num, const T* pToCopy);
	int InsertMultipleBefore(int elem, int num);
	int InsertMultipleBefore(int elem, int num, const T* pToCopy);
	int InsertMultipleAfter(int elem, int num);

	// Calls RemoveAll() then AddMultipleToTail.
	// SetSize is a synonym for SetCount
	void SetSize(int size);
	// SetCount deletes the previous contents of the container and sets the
	// container to have this many elements.
	// Use GetCount to retrieve the current count.
	void SetCount(int count);
	void SetCountNonDestructively(int count); //sets count by adding or removing elements to tail TODO: This should probably be the default behavior for SetCount

	// Calls SetSize and copies each element.
	void CopyArray(const T* pArray, int size);

	// Fast swap
	void Swap(UtlVector< T, A >& vec);

	// Add the specified array to the tail.
	int AddVectorToTail(UtlVector<T, A> const& src);

	// Finds an element (element needs operator== defined)
	int Find(const T& src) const;

	// Helper to find using std::find_if with a predicate
	//   e.g. [] -> bool ( T &a ) { return a.IsTheThingIWant(); }
	//
	// Useful if your object doesn't define a ==
	template < typename F >
	int FindPredicate(F&& predicate) const;

	void FillWithValue(const T& src);

	bool HasElement(const T& src) const;

	// Makes sure we have enough memory allocated to store a requested # of elements
	// Use NumAllocated() to retrieve the current capacity.
	void EnsureCapacity(int num);

	// Makes sure we have at least this many elements
	// Use GetCount to retrieve the current count.
	void EnsureCount(int num);

	// Element removal
	void FastRemove(int elem);	// doesn't preserve order
	void Remove(int elem);		// preserves order, shifts elements
	bool FindAndRemove(const T& src);	// removes first occurrence of src, preserves order, shifts elements
	bool FindAndFastRemove(const T& src);	// removes first occurrence of src, doesn't preserve order
	void RemoveMultiple(int elem, int num);	// preserves order, shifts elements
	void RemoveMultipleFromHead(int num); // removes num elements from tail
	void RemoveMultipleFromTail(int num); // removes num elements from tail
	void RemoveAll();				// doesn't deallocate memory

	// Memory deallocation
	void Purge();

	// Purges the list and calls delete on each element in it.
	void PurgeAndDeleteElements();

	// Compacts the vector to the number of elements actually in use 
	void Compact();

	// Set the size by which it grows when it needs to allocate more memory.
	void SetGrowSize(int size) { m_Memory.SetGrowSize(size); }

	int NumAllocated() const;	// Only use this if you really know what you're doing!

	// reverse the order of elements
	void Reverse();

protected:
	// Can't copy this unless we explicitly do it!
	UtlVector(UtlVector const& vec) { }

	// Grows the vector
	void GrowVector(int num = 1);

	// Shifts elements....
	void ShiftElementsRight(int elem, int num = 1);
	void ShiftElementsLeft(int elem, int num = 1);

	CAllocator m_Memory;
	int m_Size;

	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	T* m_Elements;

	inline void ResetDbgInfo()
	{
		m_Elements = Base();
	}
};


//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------
template<typename T, class A>
inline UtlVector<T, A>::UtlVector(int growSize, int initSize) :
	m_Memory(growSize, initSize), m_Size(0)
{
	ResetDbgInfo();
}

template<typename T, class A>
inline UtlVector<T, A>::UtlVector(T* pMemory, int allocationCount, int numElements) :
	m_Memory(pMemory, allocationCount), m_Size(numElements)
{
	ResetDbgInfo();
}

template<typename T, class A>
inline UtlVector<T, A>::~UtlVector()
{
	Purge();
}

template<typename T, class A>
inline UtlVector<T, A>& UtlVector<T, A>::operator=(const UtlVector<T, A>& other)
{
	int nCount = other.Count();
	SetSize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		(*this)[i] = other[i];
	}
	return *this;
}

//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template<typename T, class A>
inline T& UtlVector<T, A>::operator[](int i)
{
	return m_Memory[i];
}

template<typename T, class A>
inline const T& UtlVector<T, A>::operator[](int i) const
{
	return m_Memory[i];
}

template<typename T, class A>
inline T& UtlVector<T, A>::Element(int i)
{
	return m_Memory[i];
}

template<typename T, class A>
inline const T& UtlVector<T, A>::Element(int i) const
{
	return m_Memory[i];
}

template<typename T, class A>
inline T& UtlVector<T, A>::Head()
{
	return m_Memory[0];
}

template<typename T, class A>
inline const T& UtlVector<T, A>::Head() const
{
	return m_Memory[0];
}

template<typename T, class A>
inline T& UtlVector<T, A>::Tail()
{
	return m_Memory[m_Size - 1];
}

template<typename T, class A>
inline const T& UtlVector<T, A>::Tail() const
{
	return m_Memory[m_Size - 1];
}



template<typename T, class A>
inline int UtlVector<T, A>::Count() const
{
	return m_Size;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Reverse - reverse the order of elements, akin to std::vector<>::reverse()
//-----------------------------------------------------------------------------
template<typename T, class A>
void UtlVector<T, A>::Reverse()
{
	for (int i = 0; i < m_Size / 2; i++)
	{
		V_swap(m_Memory[i], m_Memory[m_Size - 1 - i]);
	}
}


//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template<typename T, class A>
inline bool UtlVector<T, A>::IsValidIndex(int i) const
{
	return (i >= 0) && (i < m_Size);
}


//-----------------------------------------------------------------------------
// Returns in invalid index
//-----------------------------------------------------------------------------
template<typename T, class A>
inline int UtlVector<T, A>::InvalidIndex()
{
	return -1;
}


//-----------------------------------------------------------------------------
// Grows the vector
//-----------------------------------------------------------------------------
template<typename T, class A>
void UtlVector<T, A>::GrowVector(int num)
{
	if (m_Size + num > m_Memory.NumAllocated())
	{
		m_Memory.Grow(m_Size + num - m_Memory.NumAllocated());
	}

	m_Size += num;
	ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// Makes sure we have enough memory allocated to store a requested # of elements
//-----------------------------------------------------------------------------
template<typename T, class A>
void UtlVector<T, A>::EnsureCapacity(int num)
{
	m_Memory.EnsureCapacity(num);
	ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// Makes sure we have at least this many elements
//-----------------------------------------------------------------------------
template<typename T, class A>
void UtlVector<T, A>::EnsureCount(int num)
{
	if (Count() < num)
	{
		AddMultipleToTail(num - Count());
	}
}


//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template<typename T, class A>
void UtlVector<T, A>::ShiftElementsRight(int elem, int num)
{
	int numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
		memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
}

template<typename T, class A>
void UtlVector<T, A>::ShiftElementsLeft(int elem, int num)
{
	int numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
	{
		memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));
	}
}


//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template<typename T, class A>
inline int UtlVector<T, A>::AddToHead()
{
	return InsertBefore(0);
}

template<typename T, class A>
inline int UtlVector<T, A>::AddToTail()
{
	return InsertBefore(m_Size);
}

template<typename T, class A>
inline T* UtlVector<T, A>::AddToTailGetPtr()
{
	return &Element(AddToTail());
}

template<typename T, class A>
inline int UtlVector<T, A>::InsertAfter(int elem)
{
	return InsertBefore(elem + 1);
}

template<typename T, class A>
int UtlVector<T, A>::InsertBefore(int elem)
{
	GrowVector();
	ShiftElementsRight(elem);
	Utils::VAlloc::Construct(&Element(elem));
	return elem;
}


//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template<typename T, class A>
inline int UtlVector<T, A>::AddToHead(const T& src)
{
	return InsertBefore(0, src);
}

template<typename T, class A>
inline int UtlVector<T, A>::AddToTail(const T& src)
{
	return InsertBefore(m_Size, src);
}

template<typename T, class A>
inline int UtlVector<T, A>::InsertAfter(int elem, const T& src)
{
	return InsertBefore(elem + 1, src);
}

template<typename T, class A>
int UtlVector<T, A>::InsertBefore(int elem, const T& src)
{
	GrowVector();
	ShiftElementsRight(elem);
	Utils::VAlloc::CopyConstruct(&Element(elem), src);
	return elem;
}


//-----------------------------------------------------------------------------
// Adds multiple elements, uses default constructor
//-----------------------------------------------------------------------------
template<typename T, class A>
inline int UtlVector<T, A>::AddMultipleToHead(int num)
{
	return InsertMultipleBefore(0, num);
}

template<typename T, class A>
inline int UtlVector<T, A>::AddMultipleToTail(int num)
{
	return InsertMultipleBefore(m_Size, num);
}

template<typename T, class A>
inline int UtlVector<T, A>::AddMultipleToTail(int num, const T* pToCopy)
{
	return InsertMultipleBefore(m_Size, num, pToCopy);
}

template<typename T, class A>
int UtlVector<T, A>::InsertMultipleAfter(int elem, int num)
{
	return InsertMultipleBefore(elem + 1, num);
}


template<typename T, class A>
void UtlVector<T, A>::SetCount(int count)
{
	RemoveAll();
	AddMultipleToTail(count);
}

template<typename T, class A>
inline void UtlVector<T, A>::SetSize(int size)
{
	SetCount(size);
}

template<typename T, class A>
void UtlVector<T, A>::SetCountNonDestructively(int count)
{
	int delta = count - m_Size;
	if (delta > 0) AddMultipleToTail(delta);
	else if (delta < 0) RemoveMultipleFromTail(-delta);
}

template<typename T, class A>
void UtlVector<T, A>::CopyArray(const T* pArray, int size)
{
	// Can't insert something that's in the list... reallocation may hose us
	Assert((Base() == NULL) || !pArray || (Base() >= (pArray + size)) || (pArray >= (Base() + Count())));

	SetSize(size);
	for (int i = 0; i < size; i++)
	{
		(*this)[i] = pArray[i];
	}
}

template<typename T, class A>
void UtlVector<T, A>::Swap(UtlVector< T, A >& vec)
{
	m_Memory.Swap(vec.m_Memory);
	std::swap(m_Size, vec.m_Size);
	std::swap(m_Elements, vec.m_Elements);
}

template<typename T, class A>
int UtlVector<T, A>::AddVectorToTail(UtlVector const& src)
{
	int base = Count();

	// Make space.
	int nSrcCount = src.Count();
	EnsureCapacity(base + nSrcCount);

	// Copy the elements.	
	m_Size += nSrcCount;
	for (int i = 0; i < nSrcCount; i++)
	{
		Utils::VAlloc::CopyConstruct(&Element(base + i), src[i]);
	}
	return base;
}

template<typename T, class A>
inline int UtlVector<T, A>::InsertMultipleBefore(int elem, int num)
{
	if (num == 0)
		return elem;

	GrowVector(num);
	ShiftElementsRight(elem, num);

	// Invoke default constructors
	for (int i = 0; i < num; ++i)
	{
		Utils::VAlloc::Construct(&Element(elem + i));
	}

	return elem;
}

template<typename T, class A>
inline int UtlVector<T, A>::InsertMultipleBefore(int elem, int num, const T* pToInsert)
{
	if (num == 0)
		return elem;

	GrowVector(num);
	ShiftElementsRight(elem, num);

	// Invoke default constructors
	if (!pToInsert)
	{
		for (int i = 0; i < num; ++i)
		{
			Utils::VAlloc::Construct(&Element(elem + i));
		}
	}
	else
	{
		for (int i = 0; i < num; i++)
		{
			Utils::VAlloc::CopyConstruct(&Element(elem + i), pToInsert[i]);
		}
	}

	return elem;
}


//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template<typename T, class A>
int UtlVector<T, A>::Find(const T& src) const
{
	for (int i = 0; i < Count(); ++i)
	{
		if (Element(i) == src)
			return i;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Finds an element using a predicate, using std::find_if
//-----------------------------------------------------------------------------
template<typename T, class A>
template< class F >
int UtlVector<T, A>::FindPredicate(F&& predicate) const
{
	const T* begin = Base();
	const T* end = begin + Count();
	const T* const& elem = std::find_if(begin, end, predicate);

	if (elem != end)
	{
		return std::distance(begin, elem);
	}

	return InvalidIndex();
}

template<typename T, class A>
void UtlVector<T, A>::FillWithValue(const T& src)
{
	for (int i = 0; i < Count(); i++)
	{
		Element(i) = src;
	}
}

template<typename T, class A>
bool UtlVector<T, A>::HasElement(const T& src) const
{
	return (Find(src) >= 0);
}


//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template<typename T, class A>
void UtlVector<T, A>::FastRemove(int elem)
{
	// Global scope to resolve conflict with Scaleform 4.0
	VAlloc::Destruct(&Element(elem));
	if (m_Size > 0)
	{
		if (elem != m_Size - 1)
			memcpy(&Element(elem), &Element(m_Size - 1), sizeof(T));
		--m_Size;
	}
}

template<typename T, class A>
void UtlVector<T, A>::Remove(int elem)
{
	// Global scope to resolve conflict with Scaleform 4.0
	VAlloc::Destruct(&Element(elem));
	ShiftElementsLeft(elem);
	--m_Size;
}

template<typename T, class A>
bool UtlVector<T, A>::FindAndRemove(const T& src)
{
	int elem = Find(src);
	if (elem != -1)
	{
		Remove(elem);
		return true;
	}
	return false;
}

template<typename T, class A>
bool UtlVector<T, A>::FindAndFastRemove(const T& src)
{
	int elem = Find(src);
	if (elem != -1)
	{
		FastRemove(elem);
		return true;
	}
	return false;
}

template<typename T, class A>
void UtlVector<T, A>::RemoveMultiple(int elem, int num)
{
	// Global scope to resolve conflict with Scaleform 4.0
	for (int i = elem + num; --i >= elem; )
		VAlloc::Destruct(&Element(i));

	ShiftElementsLeft(elem, num);
	m_Size -= num;
}

template<typename T, class A>
void UtlVector<T, A>::RemoveMultipleFromHead(int num)
{
	// Global scope to resolve conflict with Scaleform 4.0
	for (int i = num; --i >= 0; )
		VAlloc::Destruct(&Element(i));

	ShiftElementsLeft(0, num);
	m_Size -= num;
}

template<typename T, class A>
void UtlVector<T, A>::RemoveMultipleFromTail(int num)
{
	// Global scope to resolve conflict with Scaleform 4.0
	for (int i = m_Size - num; i < m_Size; i++)
		VAlloc::Destruct(&Element(i));

	m_Size -= num;
}

template<typename T, class A>
void UtlVector<T, A>::RemoveAll()
{
	for (int i = m_Size; --i >= 0; )
	{
		// Global scope to resolve conflict with Scaleform 4.0
		VAlloc::Destruct(&Element(i));
	}

	m_Size = 0;
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------

template<typename T, class A>
inline void UtlVector<T, A>::Purge()
{
	RemoveAll();
	m_Memory.Purge();
	ResetDbgInfo();
}


template<typename T, class A>
inline void UtlVector<T, A>::PurgeAndDeleteElements()
{
	for (int i = 0; i < m_Size; i++)
	{
		delete Element(i);
	}
	Purge();
}

template<typename T, class A>
inline void UtlVector<T, A>::Compact()
{
	m_Memory.Purge(m_Size);
}

template<typename T, class A>
inline int UtlVector<T, A>::NumAllocated() const
{
	return m_Memory.NumAllocated();
}



TF2_NAMESPACE_END();
