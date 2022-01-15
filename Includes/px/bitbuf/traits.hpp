#pragma once

#include <vector>
#include <array>

namespace px
{
    template<
        typename _Containter,
        size_t _Size,
        bool _IsRead,
        bool _IsWrite
    >
    struct bitbuf_traits_t
    {
        using container_type = typename _Containter;
        using block_type     = typename container_type::value_type;
        using difference_type = typename container_type::difference_type;

        static constexpr difference_type npos = -1;

        static constexpr size_t size_of_block = sizeof(block_type);
        static constexpr size_t bits_per_block = 8;
        static constexpr size_t size_per_block = size_of_block * 8;

        static constexpr size_t block_count = _Size;
        static constexpr bool is_dynamic_block = block_count == 0;

        static constexpr bool is_read   = _IsRead;
        static constexpr bool is_write  = _IsWrite;
    };

    template<bool _IsRead = true, bool _IsWrite = true>
    using bitbuf_default_traits     = bitbuf_traits_t<std::vector<uint8_t, std::allocator<uint8_t>>, 0, _IsRead, _IsWrite>;

    template<bool _IsRead = true, bool _IsWrite = true>
    using bitbuf_default_pmr_traits = bitbuf_traits_t<std::pmr::vector<uint8_t>, 0, _IsRead, _IsWrite>;

    template<size_t _Size, bool _IsRead = true, bool _IsWrite = true>
    using bitbuf_default_static_traits = bitbuf_traits_t<std::array<uint8_t, _Size>, _Size, _IsRead, _IsWrite>;

    enum class bit_type : char { zero, one };

    namespace literals
    {
        constexpr bit_type operator""_bit(const char* str, size_t len) noexcept
        {
            return (len == 1 && str[0] == '1') ? bit_type::one : bit_type::zero;
        }
    }
}
