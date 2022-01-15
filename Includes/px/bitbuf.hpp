#pragma once

#include "bitbuf/traits.hpp"
#include "bitbuf/iterator.hpp"

namespace px
{
    /// <summary>
    /// A static/dynamic storage of bits
    /// </summary>
    template<typename _BitBuf_Traits>
    class bitbuf_t
    {
        template<typename _OtherBuf_Traits>
        friend class bitbuf_t;

    public:
        using traits_type = _BitBuf_Traits;
        using container_type = typename traits_type::container_type;

        static constexpr bool is_dynamic_block = traits_type::is_dynamic_block;

        static constexpr size_t bits_per_block = traits_type::bits_per_block;
        static constexpr size_t size_per_block = traits_type::size_per_block;

        static constexpr size_t npos = static_cast<size_t>(-1);

        constexpr bitbuf_t() = default;

        constexpr bitbuf_t(size_t capacity) noexcept
        {
            if constexpr (is_dynamic_block)
                m_Storage.resize(capacity);
        }

        template<typename _OtherBuf_Traits>
        constexpr bitbuf_t(const bitbuf_t<_OtherBuf_Traits>& buf) noexcept :
            m_Storage(buf.m_Storage),
            m_ReadPos(buf.m_ReadPos),
            m_WritePos(buf.m_WritePos)
        { }

        template<typename _OtherBuf_Traits>
        constexpr bitbuf_t& operator=(const bitbuf_t<_OtherBuf_Traits>& buf) noexcept
        {
            if (this != &buf)
            {
                m_Storage = buf.m_Storage;
                m_ReadPos = buf.m_ReadPos;
                m_WritePos = buf.m_WritePos;
            }
            return *this;
        }

        template<typename _OtherBuf_Traits>
        constexpr bitbuf_t(bitbuf_t<_OtherBuf_Traits>&& buf) noexcept :
            m_Storage(std::move(buf.m_Storage)),
            m_ReadPos(buf.m_ReadPos),
            m_WritePos(buf.m_WritePos)
        { }

        template<typename _OtherBuf_Traits>
        constexpr bitbuf_t& operator=(bitbuf_t<_OtherBuf_Traits>&& buf) noexcept
        {
            if (this != &buf)
            {
                m_Storage = std::move(buf.m_Storage);
                m_ReadPos = m_ReadPos;
                m_WritePos = m_WritePos;
            }
            return *this;
        }

    public:
        template<typename _Ty>
            requires (traits_type::is_write)
        constexpr bitbuf_t& operator<<(const _Ty& data)
        {
            if constexpr (std::is_same_v<_Ty, bit_type>)
            {
                ensure_size(1);
                write_bit(data);
            }
            else if constexpr (std::is_integral_v<_Ty>)
            {
                ensure_size(sizeof(_Ty) * bits_per_block);
                for (size_t i = 0; i < sizeof(_Ty) * bits_per_block; i++)
                    write_bit((data >> i) & 1 ? bit_type::one : bit_type::zero);
            }
            else if constexpr (std::is_floating_point_v<_Ty>)
            {
                std::array bytes = std::bit_cast<std::array<unsigned char, sizeof(_Ty)>>(data);
                ensure_size(sizeof(_Ty) * bits_per_block);
                for (uint8_t byte : bytes)
                {
                    for (size_t i = 0; i < sizeof(uint8_t) * bits_per_block; i++)
                        write_bit((byte >> i) & 1 ? bit_type::one : bit_type::zero);
                }
            }
            else if constexpr (
                std::is_same_v<_Ty, char*>      || std::is_same_v<std::decay_t<_Ty>, char*> ||
                std::is_same_v<_Ty, char8_t*>   || std::is_same_v<std::decay_t<_Ty>, char8_t*> ||
                std::is_same_v<_Ty, wchar_t*>   || std::is_same_v<std::decay_t<_Ty>, wchar_t*> ||
                std::is_same_v<_Ty, char16_t*>  || std::is_same_v<std::decay_t<_Ty>, char16_t*> ||
                std::is_same_v<_Ty, char32_t*>  || std::is_same_v<std::decay_t<_Ty>, char32_t*>
            )
            {
                 (*this) << std::basic_string_view<std::remove_pointer_t<std::decay_t<_Ty>>>{ data };
            }
            else if constexpr (
                std::is_same_v<_Ty, std::string>    || std::is_same_v<_Ty, std::string_view> ||
                std::is_same_v<_Ty, std::u8string>  || std::is_same_v<_Ty, std::u8string_view> ||
                std::is_same_v<_Ty, std::wstring>   || std::is_same_v<_Ty, std::wstring_view> ||
                std::is_same_v<_Ty, std::u16string> || std::is_same_v<_Ty, std::u16string_view> ||
                std::is_same_v<_Ty, std::u32string> || std::is_same_v<_Ty, std::u32string_view>
            )
            {
                using value_type = _Ty::value_type;
                ensure_size((data.size() + 1) * sizeof(_Ty::value_type) * bits_per_block);

                for (auto c : data)
                {
                    for (size_t i = 0; i < sizeof(value_type) * bits_per_block; i++)
                        write_bit((c >> i) & 1 ? bit_type::one : bit_type::zero);
                }

                // null terminator
                for (size_t i = 0; i < sizeof(value_type) * bits_per_block; i++)
                    write_bit(bit_type::zero);
            }
            else if constexpr (
                requires(const _Ty& v) { std::begin(v); std::end(v); }
            )
            {
                write(std::begin(data), std::end(data));
            }
            return *this;
        }

        template<typename _OtherBuf_Traits>
            requires (traits_type::is_write)
        constexpr bitbuf_t& operator<<(const bitbuf_t<_OtherBuf_Traits>& data)
        {
            for (auto byte : data.m_Storage)
                (*this) << byte;
            return *this;
        }

        template<typename _Ty>
            requires (traits_type::is_write)
        constexpr void write(const _Ty& val)
        {
            (*this) << val;
        }

        template<typename _IterTy>
            requires (traits_type::is_write)
        constexpr void write(_IterTy iter, const _IterTy end)
        {
            while (iter != end)
            {
                write(*iter);
                ++iter;
            }
        }


        template<typename _Ty>
            requires (traits_type::is_read)
        constexpr const bitbuf_t& operator>>(_Ty& data) const
        {
            if constexpr (std::is_same_v<_Ty, bit_type>)
                data = read_bit();
            if constexpr (std::is_integral_v<_Ty>)
            {
                for (size_t i = 0; i < sizeof(_Ty) * bits_per_block; i++)
                {
                    if (read_bit() == bit_type::one)
                        data |= static_cast<_Ty>(1ull << i);
                    else
                        data &= ~static_cast<_Ty>(1ull << i);
                }
            }
            else if constexpr (std::is_floating_point_v<_Ty>)
            {
                std::array<uint8_t, sizeof(_Ty)> bytes;
                for (uint8_t& byte : bytes)
                {
                    for (size_t i = 0; i < sizeof(uint8_t) * bits_per_block; i++)
                    {
                        if (read_bit() == bit_type::one)
                            byte |= static_cast<uint8_t>(1ull << i);
                        else
                            byte &= ~static_cast<uint8_t>(1ull << i);
                    }
                }

                data = std::bit_cast<_Ty>(bytes);
            }
            else if constexpr (
                std::is_same_v<_Ty, std::string>    || std::is_same_v<_Ty, std::string_view> ||
                std::is_same_v<_Ty, std::u8string>  || std::is_same_v<_Ty, std::u8string_view> ||
                std::is_same_v<_Ty, std::wstring>   || std::is_same_v<_Ty, std::wstring_view> ||
                std::is_same_v<_Ty, std::u16string> || std::is_same_v<_Ty, std::u16string_view> ||
                std::is_same_v<_Ty, std::u32string> || std::is_same_v<_Ty, std::u32string_view>
            )
            {
                using value_type = _Ty::value_type;
                _Ty val;

                while (true)
                {
                    value_type c{ };
                    for (size_t i = 0; i < sizeof(value_type) * bits_per_block; i++)
                    {
                        if (read_bit() == bit_type::one)
                            c |= static_cast<value_type>(1ull << i);
                        else
                            c &= ~static_cast<value_type>(1ull << i);
                    }
                    val.push_back(c);
                    if (!c)
                        break;
                }

                data = std::move(val);
            }
            else if constexpr (
                std::is_same_v<std::decay_t<_Ty>, char*> ||
                std::is_same_v<std::decay_t<_Ty>, char8_t*> ||
                std::is_same_v<std::decay_t<_Ty>, wchar_t*> ||
                std::is_same_v<std::decay_t<_Ty>, char16_t*> ||
                std::is_same_v<std::decay_t<_Ty>, char32_t*>
            )
            {
                using value_type = std::remove_pointer_t<std::decay_t<_Ty>>;
                size_t size = std::ssize(data);

                for (size_t i = 0; i < size; i++)
                {
                    value_type c{ };
                    for (size_t i = 0; i < sizeof(value_type) * bits_per_block; i++)
                    {
                        if (read_bit() == bit_type::one)
                            c |= static_cast<value_type>(1ull << i);
                        else
                            c &= ~static_cast<value_type>(1ull << i);
                    }
                    if (!(data[i] = c))
                        break;
                }
            }
            else if constexpr (
                requires(_Ty& v) { std::begin(v); std::end(v); }
            )
            {
                read(std::begin(data), std::end(data));
            }
            return *this;
        }

        template<typename _OtherBuf_Traits>
            requires (traits_type::is_read)
        constexpr const bitbuf_t& operator>>(bitbuf_t<_OtherBuf_Traits>& data)
        {
            for (auto byte : m_Storage)
                data << byte;
            return *this;
        }

        template<typename _Ty>
            requires (traits_type::is_read)
        constexpr _Ty read() const
        {
            _Ty tmp{ };
            (*this) >> tmp;
            return tmp;
        }
        
        template<typename _Ty>
            requires (traits_type::is_read)
        constexpr _Ty read(size_t size) const
        {
            _Ty tmp{ };
            tmp.reserve(size);
            while (size--)
                tmp.emplace_back(read<_Ty::value_type>());
            return tmp;
        }

        template<typename _IterTy>
            requires (traits_type::is_read)
        constexpr void read(_IterTy iter, const _IterTy end) const
        {
            while (iter != end)
            {
                (*this) >> *iter;
                ++iter;
            }
        }

    private:
        constexpr void write_bit(bit_type bit)
        {
            const size_t write_pos = write_get() / size_per_block;

            if (bit == bit_type::one)
                m_Storage[write_pos] |= (1 << (write_get() % bits_per_block));
            else
                m_Storage[write_pos] &= ~(1 << (write_get() % bits_per_block));
            ++m_WritePos;
        }

        [[nodiscard]] constexpr bit_type read_bit() const
        {
            const size_t cur_pos = m_ReadPos++;
            const size_t read_pos = cur_pos / size_per_block;

            return m_Storage[read_pos] & (1 << (cur_pos % bits_per_block)) ? bit_type::one : bit_type::zero;
        }

    public:
        [[nodiscard]] constexpr size_t size() const noexcept
        {
            return m_Storage.size();
        }

        [[nodiscard]] constexpr const container_type& get() const noexcept
        {
            return m_Storage;
        }
        
        [[nodiscard]] constexpr auto data() const noexcept
        {
            return m_Storage.data();
        }

        [[nodiscard]] constexpr auto data() noexcept
        {
            return m_Storage.data();
        }

        [[nodiscard]] constexpr size_t read_get() const noexcept
        {
            return m_ReadPos;
        }

        constexpr void read_set(size_t pos) noexcept
            requires (traits_type::is_read)
        {
            m_ReadPos = pos;
        }
        
        constexpr void read_reset() noexcept
            requires (traits_type::is_read)
        {
            read_set(0);
        }

        constexpr void read_align(size_t alignment) noexcept
            requires (traits_type::is_read)
        {
            size_t left = read_get() % bits_per_block;
            if (left)
                left = bits_per_block - left;
            size_t offset = read_get() & (alignment * bits_per_block - bits_per_block);
            if (offset)
                offset = alignment - offset;

            if (offset || left)
            {
                read_set(read_get() + offset + bits_per_block - left);
            }
        }

        [[nodiscard]] constexpr bool read_is_aligned(size_t alignment) const noexcept
        {
            return !((read_get() % bits_per_block) || read_get() & (alignment * bits_per_block - bits_per_block));
        }

        template<typename _Ty>
        [[nodiscard]] constexpr bool can_read() const noexcept
        {
            if constexpr (is_dynamic_block)
                return true;
            else return size() >= (read_get() + sizeof(_Ty));
        }

        [[nodiscard]] constexpr size_t write_get() const noexcept
        {
            return m_WritePos;
        }

        constexpr void write_set(size_t pos) noexcept
            requires (traits_type::is_write)
        {
            m_WritePos = pos;
        }

        constexpr void write_reset() noexcept
            requires (traits_type::is_write)
        {
            write_set(0);
        }

        constexpr void write_align(size_t alignment) noexcept
            requires (traits_type::is_write)
        {
            size_t left = write_get() % bits_per_block;
            if (left)
                left = bits_per_block - left;
            size_t offset = write_get() & (alignment * bits_per_block - bits_per_block);
            if (offset)
                offset = alignment - offset;

            if (offset || left)
            {
                write_set(write_get() + offset + bits_per_block - left);
            }
        }

        [[nodiscard]] constexpr bool write_is_aligned(size_t alignment) const noexcept
        {
            return !((write_get() % bits_per_block) || write_get() & (alignment * bits_per_block - bits_per_block));
        }

        template<typename _Ty>
        [[nodiscard]] constexpr bool can_write() const noexcept
        {
            if constexpr (is_dynamic_block)
                return true;
            else return size() >= (write_get() + sizeof(_Ty));
        }

        constexpr void shrink_to_fit()
        {
            m_Storage.shrink_to_fit();
        }
        
        constexpr void ensure_size(size_t bits_size)
            requires (traits_type::is_write)
        {
            if constexpr (is_dynamic_block)
            {
                if (const size_t count = write_get() + bits_size; size() * bits_per_block < count)
                    m_Storage.resize((count % bits_per_block) + (count / bits_per_block));
            }
        }

        [[nodiscard]] constexpr bit_type operator[](size_t index) const noexcept
            requires (traits_type::is_read)
        {
            return (data()[index / traits_type::size_per_block] & (1 << (index % traits_type::size_per_block))) ? bit_type::one : bit_type::zero;
        }

        [[nodiscard]] constexpr auto read_iterator(size_t begin, size_t end) const noexcept
            requires (traits_type::is_read)
        {
            return bitbuf_const_iterator_t<bitbuf_t>::impl(this, begin, end);
        }

        [[nodiscard]] constexpr auto write_iterator(size_t begin, size_t end) const noexcept
            requires (traits_type::is_write)
        {
            return bitbuf_const_iterator_t<bitbuf_t>::impl(this, begin, end);
        }

    private:
        container_type m_Storage{ };

        mutable size_t m_ReadPos{ };
        size_t m_WritePos{ };
    };

    using ibitbuf    = bitbuf_t<bitbuf_default_traits<true, false>>;
    using obitbuf    = bitbuf_t<bitbuf_default_traits<false, true>>;
    using bitbuf     = bitbuf_t<bitbuf_default_traits<>>;

    //using bitbuf_iterator       = bitbuf_iterator_t<bitbuf>;
    using bitbuf_const_iterator = bitbuf_const_iterator_t<bitbuf>;

    using ibitbuf_pmr    = bitbuf_t<bitbuf_default_pmr_traits<true, false>>;
    using obitbuf_pmr    = bitbuf_t<bitbuf_default_pmr_traits<false, true>>;
    using bitbuf_pmr     = bitbuf_t<bitbuf_default_pmr_traits<>>;
    
    //using bitbuf_pmr_iterator       = bitbuf_iterator_t<bitbuf>;
    using bitbuf_pmr_const_iterator = bitbuf_const_iterator_t<bitbuf>;

    template<size_t _Size>  using static_ibitbuf = bitbuf_t<bitbuf_default_static_traits<_Size, true, false>>;
    template<size_t _Size>  using static_obitbuf = bitbuf_t<bitbuf_default_static_traits<_Size, false, true>>;
    template<size_t _Size>  using static_bitbuf  = bitbuf_t<bitbuf_default_static_traits<_Size>>;

    //template<size_t _Size>  using bitbuf_static_iterator       = bitbuf_iterator_t<static_bitbuf<_Size>>;
    template<size_t _Size>  using bitbuf_static_const_iterator = bitbuf_const_iterator_t<static_bitbuf<_Size>>;
}
