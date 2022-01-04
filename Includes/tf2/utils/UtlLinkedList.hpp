#pragma once

#include "UtlMemory.hpp"
#include <list>

TF2_NAMESPACE_BEGIN(::utils);

//-----------------------------------------------------------------------------
// class UtlLinkedList:
// description:
//		A lovely index-based linked list! _Ty is the typename Type, _IdxTy is the index
//		type, which usually should be an unsigned short or smaller. However,
//		you must avoid using 16- or 8-bit arithmetic on PowerPC architectures; 
//		therefore you should not use UtlLinkedListElem_t::_IdxTy as the type of 
//		a local variable... ever. PowerPC integer arithmetic must be 32- or 
//		64-bit only; otherwise performance plummets.
//-----------------------------------------------------------------------------

template<class _Ty, class _IdxTy>
struct UtlLinkedListElem_t
{
	_Ty		m_Element;
	_IdxTy	m_Previous;
	_IdxTy	m_Next;
};


// Class _STy is the storage type; the type you can use to save off indices in 
// persistent memory. Class _IdxTy is the iterator type, which is what should be used
// in local scopes. _IdxTy defaults to be _STy, but be aware that on the 360, 16-bit 
// arithmetic is catastrophically slow. Therefore you should try to save shorts
// in memory, but always operate on 32's or 64's in local scope.
// The ideal parameter order would be TSMI (you are more likely to override _MTy than _IdxTy)
// but since _MTy depends on _IdxTy we can't have the defaults in that order, alas.
template<class _Ty, class _STy = unsigned short, bool _IsMultiList = false, class _IdxTy = _STy, class _MTy = UtlMemory<UtlLinkedListElem_t<_Ty, _STy>, _IdxTy>>
class UtlLinkedList
{
public:
	/*
	using index_type = _STy;
	using value_type = _Ty;*/

	using value_type = _Ty;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;

	using index_type = _STy;
	using local_index_type = _IdxTy;
	using allocator_type = _MTy;
	static constexpr bool IsUtlLinkedList = true; // Used to match this at compiletime 

	// constructor, destructor
	UtlLinkedList(int growSize = 0, int initSize = 0);
	~UtlLinkedList();
	UtlLinkedList(const UtlLinkedList&) = delete;

	// gets particular elements
	_Ty& at(_IdxTy i);
	const _Ty& at(_IdxTy i) const;
	_Ty& operator[](_IdxTy i);
	const _Ty& operator[](_IdxTy i) const;

	// Make sure we have a particular amount of memory
	void reserve(int num);

	void set_grow_size(int growSize);

	// Memory deallocation
	void destroy();

	// Delete all the elements then call destroy.
	void destroy_and_delete();

	// Insertion methods....
	_IdxTy	push_before(_IdxTy before);
	_IdxTy	push_after(_IdxTy after);
	_IdxTy	push_to_head();
	_IdxTy	push_to_tail();

	_IdxTy	push_before(_IdxTy before, const _Ty& src);
	_IdxTy	push_after(_IdxTy after, const _Ty& src);
	_IdxTy	push_to_head(const _Ty& src);
	_IdxTy	push_to_tail(const _Ty& src);

	// find an element and return its index or invalid_index() if it couldn't be found.
	_IdxTy		find(const _Ty& src) const;

	// Look for the element. If it exists, remove it and return true. Otherwise, return false.
	bool	find_and_erase(const _Ty& src);

	// Removal methods
	void	erase(_IdxTy elem);
	void	clear();

	// Allocation/deallocation methods
	// If multilist == true, then list list may contain many
	// non-connected lists, and exists and head + tail are meaningless...
	_IdxTy	alloc(bool multilist = false);
	void	free(_IdxTy elem);

	// list modification
	void	link_before(_IdxTy before, _IdxTy elem);
	void	link_after(_IdxTy after, _IdxTy elem);
	void	unlink(_IdxTy elem);
	void	link_to_head(_IdxTy elem);
	void	link_to_tail(_IdxTy elem);

	// invalid index (_MTy will never allocate an element at this index)
	inline static _STy  invalid_index() { return static_cast<_STy>(_MTy::invalid_index()); }

	// Is a given index valid to use? (representible by _STy and not the invalid index)
	static bool in_range(_IdxTy index);

	inline static size_t element_size() { return sizeof(ListElem_t); }

	// list statistics
	int	size() const;
	inline bool empty() const
	{
		return (head() == invalid_index());
	}

	_IdxTy	max_index() const;
	_IdxTy	allocated_size() const { return m_NumAlloced; }

	// Traversing the list
	_IdxTy  head() const;
	_IdxTy  tail() const;
	_IdxTy  previous(_IdxTy i) const;
	_IdxTy  next(_IdxTy i) const;

	struct const_iterator
	{
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = _Ty;
		using difference_type = std::ptrdiff_t;
		using pointer = const value_type*;
		using reference = const value_type&;

		const_iterator(const UtlLinkedListElem_t<_Ty, _STy>* elements = nullptr) noexcept :
			m_Ptr(elements) { }

		_NODISCARD reference operator*() const noexcept
		{
			return this->m_Ptr->m_Element;
		}

		_NODISCARD pointer operator->() const noexcept
		{
			return std::pointer_traits<pointer>::pointer_to(**this);
		}

		const_iterator& operator++() noexcept
		{
			this->m_Ptr = this->_Ptr->m_Next;
			return *this;
		}

		const_iterator operator++(int) noexcept
		{
			iterator tmp = *this;
			++* this;
			return tmp;
		}

		const_iterator& operator--() noexcept
		{
			const auto new_ptr = this->m_Ptr->m_Previous;
			this->m_Ptr = new_ptr;
			return *this;
		}

		const_iterator operator--(int) noexcept
		{
			iterator tmp = *this;
			--*this;
			return tmp;
		}

		_NODISCARD bool operator==(const const_iterator& right) const noexcept
		{
			return this->m_Ptr == right.m_Ptr;
		}

		_NODISCARD bool operator!=(const const_iterator& right) const noexcept
		{
			return !(*this == m_Ptr->m_Element);
		}

	private:
		const UtlLinkedListElem_t<_Ty, _STy>* m_Ptr;
	};

	struct iterator
	{
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = _Ty;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		iterator(const UtlLinkedListElem_t<_Ty, _STy>* elements = nullptr) noexcept :
			m_Ptr(elements) { }

		_NODISCARD reference operator*() const noexcept
		{
			return this->m_Ptr->m_Element;
		}

		_NODISCARD pointer operator->() const noexcept
		{
			return std::pointer_traits<pointer>::pointer_to(**this);
		}

		iterator& operator++() noexcept
		{
			this->m_Ptr = this->_Ptr->m_Next;
			return *this;
		}

		iterator operator++(int) noexcept
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		iterator& operator--() noexcept
		{
			const auto new_ptr = this->m_Ptr->m_Previous;
			this->m_Ptr = new_ptr;
			return *this;
		}

		iterator operator--(int) noexcept
		{
			iterator tmp = *this;
			--*this;
			return tmp;
		}

		_NODISCARD bool operator==(const iterator& right) const noexcept
		{
			return this->m_Ptr->m_Element == right.m_Ptr->m_Element;
		}

		_NODISCARD bool operator!=(const iterator& right) const noexcept
		{
			return !(*this == right);
		}

	private:
		UtlLinkedListElem_t<_Ty, _STy>* m_Ptr;
	};

	const_iterator begin() const
	{
		return const_iterator(this->at_internal(head()));
	}
	iterator begin()
	{
		return iterator(this->at_internal(head()));
	}

	const_iterator end() const
	{
		return const_iterator(this->at_internal(invalid_index()));
	}
	iterator end()
	{
		return iterator(this->at_internal(invalid_index()));
	}

	// Are nodes in the list or valid?
	bool  is_valid(_IdxTy i) const;
	bool  exists(_IdxTy i) const;

protected:
	// What the linked list element looks like
	using ListElem_t = UtlLinkedListElem_t<_Ty, _STy>;

	// constructs the class
	_IdxTy		alloc_internal(bool multilist = false);
	void build_list();

	// Gets at the list element....
	ListElem_t& at_internal(_IdxTy i) { return m_Memory[i]; }
	const ListElem_t& at_internal(_IdxTy i) const { return m_Memory[i]; }

	_MTy	m_Memory;
	_IdxTy	m_Head;
	_IdxTy	m_Tail;
	_IdxTy	m_FirstFree;
	_IdxTy	m_ElementCount;		// The number actually in the list
	_IdxTy	m_NumAlloced;		// The number of allocated elements
	typename _MTy::Iterator_t	m_LastAlloc; // the last index allocated

	// For debugging purposes; 
	// it's in release builds so this can be used in libraries correctly
	ListElem_t* m_DbgElements;

	_MTy const& Memory() const
	{
		return m_Memory;
	}

	void ResetDbgInfo()
	{
		m_DbgElements = m_Memory.data();
	}

private:
	// Faster version of next that can only be used from tested code internal
	// to this class, such as find(). It avoids the cost of checking the index
	// validity, which is a big win on debug builds.
	_IdxTy  next_internal(_IdxTy i) const;
};



//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::UtlLinkedList(int growSize, int initSize) :
	m_Memory(growSize, initSize), m_LastAlloc(m_Memory.invalid_iterator())
{
	// Prevent signed non-int datatypes
	static_assert(sizeof(_STy) == 4 || (((_STy)-1) > 0));
	build_list();
	ResetDbgInfo();
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::~UtlLinkedList()
{
	clear();
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::build_list()
{
	m_Head = invalid_index();
	m_Tail = invalid_index();
	m_FirstFree = invalid_index();
	m_ElementCount = 0;
	m_NumAlloced = 0;
}


//-----------------------------------------------------------------------------
// gets particular elements
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _Ty& UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::at(_IdxTy i)
{
	return at_internal(i).m_Element;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline const _Ty& UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::at(_IdxTy i) const
{
	return at_internal(i).m_Element;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _Ty& UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::operator[](_IdxTy i)
{
	return at_internal(i).m_Element;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline const _Ty& UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::operator[](_IdxTy i) const
{
	return at_internal(i).m_Element;
}

//-----------------------------------------------------------------------------
// list statistics
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline int UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::size() const
{
	return m_ElementCount;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::max_index() const
{
	return m_Memory.allocated_size();
}


//-----------------------------------------------------------------------------
// Traversing the list
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::head() const
{
	return m_Head;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::tail() const
{
	return m_Tail;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::previous(_IdxTy i) const
{
	return at_internal(i).m_Previous;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::next(_IdxTy i) const
{
	return at_internal(i).m_Next;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::next_internal(_IdxTy i) const
{
	return at_internal(i).m_Next;
}


//-----------------------------------------------------------------------------
// Are nodes in the list or valid?
//-----------------------------------------------------------------------------

#pragma warning(push)
#pragma warning( disable: 4310 ) // Allows "(_IdxTy)(_STy)_MTy::INVALID_INDEX" below
template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline bool UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::in_range(_IdxTy index) // Static method
{
	// Since _STy is not necessarily the type returned by _MTy, we need to check that _MTy returns indices
	// which are representable by _STy. A common case is '_STy === unsigned short', '_IdxTy == int', in which
	// case CUtlMemory will have 'invalid_index == (int)-1' (which casts to 65535 in _STy), and will
	// happily return elements at index 65535 and above.

	// Do some static checks here:
	//  '_IdxTy' needs to be able to store '_STy'
	static_assert(sizeof(_IdxTy) >= sizeof(_STy));
	//  '_STy' should be unsigned (to avoid signed arithmetic errors for plausibly exhaustible ranges)
	static_assert((sizeof(_STy) > 2) || (((_STy)-1) > 0));
	//  _MTy::INVALID_INDEX should be storable in _STy to avoid ambiguities (e.g. with 65536)
	static_assert((_MTy::INVALID_INDEX == -1) || (_MTy::INVALID_INDEX == static_cast<_STy>(_MTy::INVALID_INDEX)));

	return ((static_cast<_STy>(index) == index) && (static_cast<_STy>(index) != invalid_index()));
}
#pragma warning(pop)

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline bool UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::is_valid(_IdxTy i) const
{
	if (!m_Memory.is_valid_index(i))
		return false;

	if (m_Memory.is_index_after(i, m_LastAlloc))
		return false; // don't read values that have been allocated, but not constructed

	return (m_Memory[i].m_Previous != i) || (m_Memory[i].m_Next == i);
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline bool UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::exists(_IdxTy i) const
{
	if (!m_Memory.is_valid_index(i) || m_Memory.is_index_after(i, m_LastAlloc))
		return false; // don't read values that have been allocated, but not constructed

	return previous(i) != i;
}

//-----------------------------------------------------------------------------
// Makes sure we have enough memory allocated to store a requested # of elements
//-----------------------------------------------------------------------------

template< typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy >
void UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::reserve(int num)
{
	m_Memory.reserve(num);
	ResetDbgInfo();
}

template< typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy >
void UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::set_grow_size(int growSize)
{
	clear();
	m_Memory.init(growSize);
	ResetDbgInfo();
}


//-----------------------------------------------------------------------------
// Deallocate memory
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::destroy()
{
	clear();

	m_Memory.destroy();
	m_FirstFree = invalid_index();
	m_NumAlloced = 0;

	//Routing "m_LastAlloc = m_Memory.invalid_iterator();" through a local const to sidestep an internal compiler error on 360 builds
	const typename _MTy::Iterator_t scInvalidIterator = m_Memory.invalid_iterator();
	m_LastAlloc = scInvalidIterator;
	ResetDbgInfo();
}


template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::destroy_and_delete()
{
	_IdxTy iNext;
	for (_IdxTy i = head(); i != invalid_index(); i = iNext)
	{
		iNext = next(i);
		delete at(i);
	}

	destroy();
}


//-----------------------------------------------------------------------------
// Node allocation/deallocation
//-----------------------------------------------------------------------------
template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
_IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::alloc_internal(bool multilist)
{
	_IdxTy elem;
	if (m_FirstFree == invalid_index())
	{
		typename _MTy::Iterator_t it = m_Memory.is_valid_iterator(m_LastAlloc) ? m_Memory.next(m_LastAlloc) : m_Memory.first();

		if (!m_Memory.is_valid_iterator(it))
		{
			m_Memory.grow();
			ResetDbgInfo();

			it = m_Memory.is_valid_iterator(m_LastAlloc) ? m_Memory.next(m_LastAlloc) : m_Memory.first();

			if (!m_Memory.is_valid_iterator(it))
				return invalid_index();
		}

		// We can overflow before the utlmemory overflows, since _STy != _IdxTy
		if (!in_range(m_Memory.get_index(it)))
			return invalid_index();

		m_LastAlloc = it;
		elem = m_Memory.get_index(m_LastAlloc);
		m_NumAlloced++;
	}
	else
	{
		elem = m_FirstFree;
		m_FirstFree = at_internal(m_FirstFree).m_Next;
	}

	if (!multilist)
	{
		at_internal(elem).m_Next = elem;
		at_internal(elem).m_Previous = elem;
	}
	else
	{
		at_internal(elem).m_Next = invalid_index();
		at_internal(elem).m_Previous = invalid_index();
	}

	return elem;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
_IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::alloc(bool multilist)
{
	_IdxTy elem = alloc_internal(multilist);
	if (elem == invalid_index())
		return elem;

	utils::VAlloc::Construct(&at(elem));

	return elem;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::free(_IdxTy elem)
{
	unlink(elem);

	ListElem_t& internalElem = at_internal(elem);
	utils::VAlloc::Destruct(&internalElem.m_Element);
	internalElem.m_Next = m_FirstFree;
	m_FirstFree = elem;
}

//-----------------------------------------------------------------------------
// Insertion methods; allocates and links (uses default constructor)
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
_IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::push_before(_IdxTy before)
{
	// Make a new node
	_IdxTy   newNode = alloc_internal();
	if (newNode == invalid_index())
		return newNode;

	// Link it in
	link_before(before, newNode);

	// utils::VAlloc::Construct the data
	utils::VAlloc::Construct(&at(newNode));

	return newNode;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
_IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::push_after(_IdxTy after)
{
	// Make a new node
	_IdxTy   newNode = alloc_internal();
	if (newNode == invalid_index())
		return newNode;

	// Link it in
	link_after(after, newNode);

	// utils::VAlloc::Construct the data
	utils::VAlloc::Construct(&at(newNode));

	return newNode;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::push_to_head()
{
	return push_after(invalid_index());
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::push_to_tail()
{
	return push_before(invalid_index());
}


//-----------------------------------------------------------------------------
// Insertion methods; allocates and links (uses copy constructor)
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
_IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::push_before(_IdxTy before, const _Ty& src)
{
	// Make a new node
	_IdxTy   newNode = alloc_internal();
	if (newNode == invalid_index())
		return newNode;

	// Link it in
	link_before(before, newNode);

	// utils::VAlloc::Construct the data
	CopyConstruct(&at(newNode), src);

	return newNode;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
_IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::push_after(_IdxTy after, const _Ty& src)
{
	// Make a new node
	_IdxTy   newNode = alloc_internal();
	if (newNode == invalid_index())
		return newNode;

	// Link it in
	link_after(after, newNode);

	// utils::VAlloc::Construct the data
	CopyConstruct(&at(newNode), src);

	return newNode;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::push_to_head(const _Ty& src)
{
	return push_after(invalid_index(), src);
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline _IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::push_to_tail(const _Ty& src)
{
	return push_before(invalid_index(), src);
}


//-----------------------------------------------------------------------------
// Removal methods
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
_IdxTy UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::find(const _Ty& src) const
{
	// Cache the invalidIndex to avoid two levels of function calls on each iteration.
	_IdxTy invalidIndex = invalid_index();
	for (_IdxTy i = head(); i != invalidIndex; i = next_internal(i))
	{
		if (at(i) == src)
			return i;
	}
	return invalid_index();
}


template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
bool UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::find_and_erase(const _Ty& src)
{
	_IdxTy i = find(src);
	if (i == invalid_index())
	{
		return false;
	}
	else
	{
		erase(i);
		return true;
	}
}


template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::erase(_IdxTy elem)
{
	free(elem);
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::clear()
{
	// Have to do some convoluted stuff to invoke the destructor on all
	// valid elements for the multilist case (since we don't have all elements
	// connected to each other in a list).

	if (m_LastAlloc == m_Memory.invalid_iterator())
		return;

	if constexpr (_IsMultiList)
	{
		for (typename _MTy::Iterator_t it = m_Memory.First(); it != m_Memory.invalid_iterator(); it = m_Memory.next(it))
		{
			_IdxTy i = m_Memory.get_index(it);
			if (is_valid(i)) // skip elements already in the free list
			{
				ListElem_t& internalElem = at_internal(i);
				utils::VAlloc::Destruct(&internalElem.m_Element);
				internalElem.m_Previous = i;
				internalElem.m_Next = m_FirstFree;
				m_FirstFree = i;
			}

			if (it == m_LastAlloc)
				break; // don't destruct elements that haven't ever been constructed
		}
	}
	else
	{
		_IdxTy i = head();
		_IdxTy next_el;
		while (i != invalid_index())
		{
			next_el = next(i);
			ListElem_t& internalElem = at_internal(i);
			utils::VAlloc::Destruct(&internalElem.m_Element);
			internalElem.m_Previous = i;
			internalElem.m_Next = next_el == invalid_index() ? m_FirstFree : next_el;
			i = next_el;
		}
		if (head() != invalid_index())
		{
			m_FirstFree = head();
		}
	}

	// Clear everything else out
	m_Head = invalid_index();
	m_Tail = invalid_index();
	m_ElementCount = 0;
}


//-----------------------------------------------------------------------------
// list modification
//-----------------------------------------------------------------------------

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::link_before(_IdxTy before, _IdxTy elem)
{
	// unlink it if it's in the list at the moment
	unlink(elem);

	ListElem_t* pNewElem = &at_internal(elem);

	// The element *after* our newly linked one is the one we linked before.
	pNewElem->m_Next = before;

	_STy newElem_mPrevious; // we need to hang on to this for the compairson against invalid_index()
					// below; otherwise we get a a load-hit-store on pNewElem->m_Previous, even
					// with RESTRICT
	if (before == invalid_index())
	{
		// In this case, we're linking to the end of the list, so reset the tail
		newElem_mPrevious = m_Tail;
		pNewElem->m_Previous = m_Tail;
		m_Tail = elem;
	}
	else
	{
		// Here, we're not linking to the end. Set the prev pointer to point to
		// the element we're linking.
		ListElem_t* beforeElem = &at_internal(before);
		pNewElem->m_Previous = newElem_mPrevious = beforeElem->m_Previous;
		beforeElem->m_Previous = elem;
	}

	// Reset the head if we linked to the head of the list
	if (newElem_mPrevious == invalid_index())
		m_Head = elem;
	else
		at_internal(newElem_mPrevious).m_Next = elem;

	// one more element baby
	++m_ElementCount;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::link_after(_IdxTy after, _IdxTy elem)
{
	// unlink it if it's in the list at the moment
	if (exists(elem))
		unlink(elem);

	ListElem_t& newElem = at_internal(elem);

	// The element *before* our newly linked one is the one we linked after
	newElem.m_Previous = after;
	if (after == invalid_index())
	{
		// In this case, we're linking to the head of the list, reset the head
		newElem.m_Next = m_Head;
		m_Head = elem;
	}
	else
	{
		// Here, we're not linking to the end. Set the next pointer to point to
		// the element we're linking.
		ListElem_t& afterElem = at_internal(after);
		newElem.m_Next = afterElem.m_Next;
		afterElem.m_Next = elem;
	}

	// Reset the tail if we linked to the tail of the list
	if (newElem.m_Next == invalid_index())
		m_Tail = elem;
	else
		at_internal(newElem.m_Next).m_Previous = elem;

	// one more element baby
	++m_ElementCount;
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
void  UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::unlink(_IdxTy elem)
{
	if (exists(elem))
	{
		ListElem_t* pOldElem = &m_Memory[elem];

		// If we're the first guy, reset the head
		// otherwise, make our previous node's next pointer = our next
		if (pOldElem->m_Previous != invalid_index())
		{
			m_Memory[pOldElem->m_Previous].m_Next = pOldElem->m_Next;
		}
		else
		{
			m_Head = pOldElem->m_Next;
		}

		// If we're the last guy, reset the tail
		// otherwise, make our next node's prev pointer = our prev
		if (pOldElem->m_Next != invalid_index())
		{
			m_Memory[pOldElem->m_Next].m_Previous = pOldElem->m_Previous;
		}
		else
		{
			m_Tail = pOldElem->m_Previous;
		}

		// This marks this node as not in the list, 
		// but not in the free list either
		pOldElem->m_Previous = pOldElem->m_Next = elem;

		// One less puppy
		--m_ElementCount;
	}
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline void UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::link_to_head(_IdxTy elem)
{
	link_after(invalid_index(), elem);
}

template<typename _Ty, class _STy, bool _IsMultiList, class _IdxTy, class _MTy>
inline void UtlLinkedList<_Ty, _STy, _IsMultiList, _IdxTy, _MTy>::link_to_tail(_IdxTy elem)
{
	link_before(invalid_index(), elem);
}


TF2_NAMESPACE_END();