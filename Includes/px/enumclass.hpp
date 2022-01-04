#pragma once

#include <type_traits>
#include <bitset>

namespace px
{
    template<typename _ETy>
    class bitmask
    {
    public:
        using type = _ETy;
        using bitset_type = std::bitset<static_cast<size_t>(_ETy::_Highest_Enum)>;
        using bitset_reference = bitset_type::reference;

    public:
        constexpr bitmask() = default;
        constexpr bitmask(uint64_t val) noexcept : bitset(val) { }
        constexpr bitmask(uint32_t val) noexcept : bitset(static_cast<uint64_t>(val)) { }

        template <class _Ty, class _Tr, class _Alloc>
        explicit bitmask(const std::basic_string<_Ty, _Tr, _Alloc>& str,
                      typename std::basic_string<_Ty, _Tr, _Alloc>::size_type pos = 0,
                      typename std::basic_string<_Ty, _Tr, _Alloc>::size_type count = std::basic_string<_Ty, _Tr, _Alloc>::npos,
                      _Ty zero = static_cast<_Ty>('0'), _Ty one = static_cast<_Ty>('1')) : bitset(str, pos, count, zero, one)
        { }

        template <class _Ty>
        explicit bitmask(const _Ty* ntcts, typename std::basic_string<_Ty>::size_type count = std::basic_string<_Ty>::npos,
                      _Ty zero = static_cast<_Ty>('0'), _Ty one = static_cast<_Ty>('1')) : bitset(ntcts, count, zero, one)
        { }


        bitmask& set()                          noexcept    { bitset.set();                            return *this; }
        bitmask& set(type pos, bool val = true)             { bitset.set(static_cast<size_t>(pos), val); return *this; }
        _NODISCARD bitmask& flip()              noexcept    { bitset.flip();                           return *this; }
        _NODISCARD bitmask& flip(type pos)                  { bitset.flip(static_cast<size_t>(pos));   return *this; }
        _NODISCARD bitmask& reset()             noexcept    { bitset.reset();                          return *this; }
        _NODISCARD bitmask& reset(type pos)                 { bitset.reset(static_cast<size_t>(pos));  return *this; }

        _NODISCARD bool test(type pos)      const           { return bitset.test(static_cast<size_t>(pos)); }
        _NODISCARD bool any()               const noexcept  { return bitset.any(); }
        _NODISCARD bool none()              const noexcept  { return bitset.none(); }
        _NODISCARD bool all()               const noexcept  { return bitset.all(); }

        _NODISCARD size_t size()             const noexcept { return bitset.size(); }
        _NODISCARD size_t count()            const noexcept { return bitset.count(); }

        _NODISCARD bool operator[](type pos) const  { return bitset.operator[](static_cast<size_t>(pos)); }
        _NODISCARD auto operator[](type pos)        { return bitset.operator[](static_cast<size_t>(pos)); }
        _NODISCARD bool operator[](size_t pos)const { return bitset.operator[](pos); }
        _NODISCARD auto operator[](size_t pos)      { return bitset.operator[](pos); }

        template<class _Ty = char, class _Tr = std::char_traits<_Ty>, class _Alloc = std::allocator<_Ty>>
        _NODISCARD auto to_string(_Ty zero = static_cast<_Ty>('0'), _Ty one = static_cast<_Ty>('1'))
            const
        {
            return bitset.to_string(zero, one);
        }

        _NODISCARD uint64_t to_ullong()     const { return bitset.to_ullong(); }
        _NODISCARD uint32_t to_ulong()      const { return bitset.to_ulong(); }

        _NODISCARD bitset_type& get()                     noexcept { return bitset; }
        _NODISCARD operator bitset_type& ()               noexcept { return bitset; }
        _NODISCARD const bitset_type& get()         const noexcept { return bitset; }
        _NODISCARD operator const bitset_type& ()   const noexcept { return bitset; }

        _NODISCARD auto operator~()              const noexcept { return ebitset(bitset.operator~()); }
        _NODISCARD auto operator<<(type pos)     const noexcept { return ebitset(bitset.operator<<(static_cast<size_t>(pos))); }
        _NODISCARD auto operator>>(type pos)     const noexcept { return ebitset(bitset.operator>>(static_cast<size_t>(pos))); }
        _NODISCARD bool operator==(type pos)     const noexcept { return bitset.operator==(static_cast<size_t>(pos)); }

        auto operator>>=(type pos)                   noexcept { bitset.operator>>=(static_cast<size_t>(pos)); return *this; }
        auto operator<<=(type pos)                   noexcept { bitset.operator<<=(static_cast<size_t>(pos)); return *this; }
        auto operator^=(const bitset_type& right)    noexcept { bitset.operator^=(right); return *this; }
        auto operator^=(const bitmask& right)        noexcept { operator^=(right.get());  return *this; }
        auto operator|=(const bitset_type& right)    noexcept { bitset.operator|=(right); return *this; }
        auto operator|=(const bitmask& right)        noexcept { operator|=(right.get());  return *this; }
        auto operator&=(const bitset_type& right)    noexcept { bitset.operator&=(right); return *this; }
        auto operator&=(const bitmask& right)        noexcept { operator&=(right.get());  return *this; }

        _NODISCARD bool test(const bitmask& other) const
        {
            if (other.size() != size())
                return false;

            for (size_t i = 0, c = other.count(); i < size() && c > 0; ++i)
            {
                if (other[i])
                {
                    if (!bitset[i])
                        return false;
                    --c;
                }
            }

            return true;
        }

        static constexpr bitmask null() noexcept { return { }; }

    private:
        bitset_type bitset;
    };

    template<typename _ETy>
    _NODISCARD bitmask<_ETy> operator&(const bitmask<_ETy>& l, const bitmask<_ETy>& r) noexcept
    {
        auto ret = l;
        return ret &= r;
    }

    template<typename _ETy>
    _NODISCARD bitmask<_ETy> operator|(const bitmask<_ETy>& l, const bitmask<_ETy>& r) noexcept
    {
        auto ret = l;
        return ret |= r;
    }

    template<typename _ETy>
    _NODISCARD bitmask<_ETy> operator^(const bitmask<_ETy>& l, const bitmask<_ETy>& r) noexcept
    {
        auto ret = l;
        return ret ^= r;
    }

    template<typename... _ETy>
    constexpr uint64_t bitmask_or(_ETy... vals) noexcept
    {
        return ((1Ui64 << static_cast<uint64_t>(vals)) | ...);
    }

    template<typename... _ETy>
    constexpr uint64_t bitmask_xor(_ETy... vals) noexcept
    {
        return ((1Ui64 << static_cast<uint64_t>(vals)) ^ ...);
    }
    
    template<typename _ETy>
    constexpr uint64_t bitmask_not(_ETy vals) noexcept
    {
        return (~(1Ui64 << static_cast<uint64_t>(vals)));
    }

    template<typename... _ETy>
    constexpr uint64_t bitmask_and(_ETy... vals) noexcept
    {
        return ((1Ui64 << static_cast<uint64_t>(vals)) & ...);
    }
}