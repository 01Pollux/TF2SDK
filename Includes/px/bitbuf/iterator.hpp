#pragma once

#include <iterator>

namespace px
{
    template<typename _BitBuffer_Ty>
    struct bitbuf_const_iterator_t
    {
        using iterator_type     = bitbuf_const_iterator_t;
        using parent_type       = _BitBuffer_Ty;
        using traits_type       = typename _BitBuffer_Ty::traits_type;

        using iterator_category = std::random_access_iterator_tag;
        using value_type        = bit_type;
        using difference_type   = typename traits_type::difference_type;

        using reference         = typename traits_type::block_type&;
        using const_reference   = const typename traits_type::block_type&;
        using pointer           = typename traits_type::block_type*;
        using const_pointer     = const typename traits_type::block_type*;

        struct impl;

        constexpr bitbuf_const_iterator_t() = default;

        constexpr bitbuf_const_iterator_t(const parent_type* parent, size_t pos) noexcept :
            m_Parent(parent), m_BitPos(pos)
        { }

        [[nodiscard]] constexpr bit_type operator*() const noexcept
        {
            return (m_Parent->data()[index()] & (1 << bit_left())) ? bit_type::one : bit_type::zero;
        }

        constexpr size_t index() const noexcept
        {
            return bit_index() / traits_type::size_per_block;
        }

        constexpr size_t bit_left() const noexcept
        {
            return bit_index() % traits_type::size_per_block;
        }

        constexpr size_t bit_index() const noexcept
        {
            return m_BitPos;
        }

        constexpr iterator_type& operator++() noexcept
        {
            ++m_BitPos;
            return *this;
        }

        constexpr iterator_type& operator--() noexcept
        {
            --m_BitPos;
            return *this;
        }

        constexpr iterator_type operator++(int) noexcept
        {
            iterator_type tmp = *this;
            ++m_BitPos;
            return tmp;
        }

        constexpr iterator_type operator--(int) noexcept
        {
            iterator_type tmp = *this;
            --m_BitPos;
            return tmp;
        }

        constexpr iterator_type& operator+=(difference_type diff) noexcept
        {
            m_BitPos += diff;
            return *this;
        }

        [[nodiscard]] constexpr iterator_type operator+(difference_type diff) const noexcept
        {
            iterator_type tmp = *this;
            return (tmp += diff);
        }

        constexpr iterator_type& operator-=(difference_type diff) noexcept
        {
            m_BitPos -= diff;
            return *this;
        }

        [[nodiscard]] constexpr iterator_type operator-(difference_type diff) const noexcept
        {
            iterator_type tmp = *this;
            return (tmp -= diff);
        }

        [[nodiscard]] constexpr difference_type operator-(const iterator_type& iter) const noexcept
        {
            return this->m_BitPos - iter.m_BitPos;
        }

        [[nodiscard]] constexpr value_type operator[](difference_type diff) const noexcept
        {
            iterator_type tmp = *this + diff;
            return *tmp;
        }

        [[nodiscard]] constexpr bool operator==(const iterator_type& iter) const noexcept
        {
            return this->m_BitPos == iter.m_BitPos;
        }

#if _HAS_CXX20
        [[nodiscard]] constexpr std::strong_ordering operator<=>(const iterator_type& iter) const noexcept
        {
            return this->m_BitPos <=> iter.m_BitPos;
        }
#else
        [[nodiscard]] constexpr bool operator!=(const iterator_type& iter) const noexcept
        {
            return !(*this == iter);
        }

        [[nodiscard]] constexpr bool operator<(const iterator_type& iter) const noexcept
        {
            return this->m_BitPos < iter.m_BitPos;
        }

        [[nodiscard]] constexpr bool operator>(const iterator_type& iter) const noexcept
        {
            return iter.m_BitPos < *this;
        }

        [[nodiscard]] constexpr bool operator<=(const iterator_type& iter) const noexcept
        {
            return !(iter.m_BitPos < *this);
        }

        [[nodiscard]] constexpr bool operator>=(const iterator_type& iter) const noexcept
        {
            return !(*this < iter.m_BitPos);
        }
#endif

    private:
        size_t m_BitPos{ 0 };
        const parent_type* m_Parent;
    };

    template<typename _BitBuffer_Ty>
    struct bitbuf_const_iterator_t<_BitBuffer_Ty>::impl
    {
    public:
        using parent_type = _BitBuffer_Ty;
        using iterator_type = typename bitbuf_const_iterator_t<parent_type>;

        constexpr impl(const parent_type* parent, size_t begin, size_t end) noexcept :
            m_Begin(parent, begin),
            m_End(parent, end)
        { }

        constexpr auto begin() const noexcept
        {
            return m_Begin;
        }

        constexpr auto end() const noexcept
        {
            return m_End;
        }

    private:
        iterator_type m_Begin;
        iterator_type m_End;
    };
}
