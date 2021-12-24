#pragma once

#include <ranges>

#include <tf2/entity/BaseEntity.hpp>

TF2_NAMESPACE_BEGIN(::Utils);

namespace Impl::EntityIterator
{
	class const_iterator
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = IBaseEntityInternal*;
		using difference_type = std::ptrdiff_t;
		using pointer = const value_type*;
		using reference = const value_type&;

		const_iterator(unsigned short first_idx = static_cast<unsigned short>(-1)) noexcept :
			m_CurrentIndex(first_idx) { }

		SG_SDK_TF2 _NODISCARD reference operator*() const;
		_NODISCARD pointer operator->() const
		{
			return &(**this);
		}

		SG_SDK_TF2 const_iterator& operator++();
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			++*this;
			return tmp;
		}

		SG_SDK_TF2 const_iterator& operator--();
		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			--*this;
			return tmp;
		}

		_NODISCARD bool operator==(const const_iterator& right) const noexcept
		{
			return this->m_CurrentIndex == right.m_CurrentIndex;
		}

		_NODISCARD bool operator!=(const const_iterator& right) const noexcept
		{
			return !(*this == right);
		}

	private:
		unsigned short m_CurrentIndex;
	};

	class iterator
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = IBaseEntityInternal*;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		iterator(unsigned short first_idx = static_cast<unsigned short>(-1)) noexcept :
			m_CurrentIndex(first_idx) { }

		SG_SDK_TF2 _NODISCARD reference operator*() const;
		_NODISCARD pointer operator->() const
		{
			return &(**this);
		}

		SG_SDK_TF2 iterator& operator++();
		iterator operator++(int)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		SG_SDK_TF2 iterator& operator--();
		iterator operator--(int)
		{
			iterator tmp = *this;
			--*this;
			return tmp;
		}

		_NODISCARD bool operator==(const iterator& right) const noexcept
		{
			return this->m_CurrentIndex == right.m_CurrentIndex;
		}

		_NODISCARD bool operator!=(const iterator& right) const noexcept
		{
			return !(*this == right);
		}

	private:
		unsigned short m_CurrentIndex;
	};

	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
}

struct IBaseEntityIterator
{
public:
	using iterator = Impl::EntityIterator::iterator;
	using const_iterator = Impl::EntityIterator::const_iterator;
	using reverse_iterator = Impl::EntityIterator::reverse_iterator;
	using const_reverse_iterator = Impl::EntityIterator::const_reverse_iterator;

	SG_SDK_TF2 const_iterator begin() const;
	const_iterator end() const
	{
		return const_iterator();
	}

	const_iterator cbegin() const
	{
		return begin();
	}
	const_iterator cend() const
	{
		return end();
	}

	SG_SDK_TF2 iterator begin();
	iterator end()
	{
		return iterator();
	}

	SG_SDK_TF2 const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator crbegin() const
	{
		return rbegin();
	}

	const_reverse_iterator crend() const
	{
		return rend();
	}

	SG_SDK_TF2 reverse_iterator rbegin();
	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}
};

TF2_NAMESPACE_END();

template<>
inline constexpr bool std::ranges::enable_borrowed_range<TF2::Utils::IBaseEntityIterator> = true;