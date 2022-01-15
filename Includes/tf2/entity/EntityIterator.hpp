#pragma once

#include <ranges>

#include <tf2/entity/BaseEntity.hpp>

TF2_NAMESPACE_BEGIN(::utils);

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

		[[nodiscard]] PX_SDK_TF2 reference operator*() const;
		[[nodiscard]] pointer operator->() const
		{
			return &(**this);
		}

		PX_SDK_TF2 const_iterator& operator++();
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			++*this;
			return tmp;
		}

		PX_SDK_TF2 const_iterator& operator--();
		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			--*this;
			return tmp;
		}

		[[nodiscard]] bool operator==(const const_iterator& right) const noexcept
		{
			return this->m_CurrentIndex == right.m_CurrentIndex;
		}

		[[nodiscard]] bool operator!=(const const_iterator& right) const noexcept
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

		[[nodiscard]] PX_SDK_TF2 reference operator*() const;
		[[nodiscard]] pointer operator->() const
		{
			return &(**this);
		}

		PX_SDK_TF2 iterator& operator++();
		iterator operator++(int)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		PX_SDK_TF2 iterator& operator--();
		iterator operator--(int)
		{
			iterator tmp = *this;
			--*this;
			return tmp;
		}

		[[nodiscard]] bool operator==(const iterator& right) const noexcept
		{
			return this->m_CurrentIndex == right.m_CurrentIndex;
		}

		[[nodiscard]] bool operator!=(const iterator& right) const noexcept
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

	[[nodiscard]] PX_SDK_TF2 const_iterator begin() const;
	[[nodiscard]] const_iterator end() const
	{
		return const_iterator();
	}

	[[nodiscard]] const_iterator cbegin() const
	{
		return begin();
	}
	[[nodiscard]] const_iterator cend() const
	{
		return end();
	}

	[[nodiscard]] PX_SDK_TF2 iterator begin();
	[[nodiscard]] iterator end()
	{
		return iterator();
	}

	[[nodiscard]] PX_SDK_TF2 const_reverse_iterator rbegin() const;
	[[nodiscard]] const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

	[[nodiscard]] const_reverse_iterator crbegin() const
	{
		return rbegin();
	}

	[[nodiscard]] const_reverse_iterator crend() const
	{
		return rend();
	}

	[[nodiscard]] PX_SDK_TF2 reverse_iterator rbegin();
	[[nodiscard]] reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}
};

TF2_NAMESPACE_END();

template<>
inline constexpr bool std::ranges::enable_borrowed_range<tf2::utils::IBaseEntityIterator> = true;