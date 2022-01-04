#pragma once

#include <string>
#include <array>

#define PX_NAMESPACE_BEGIN(...)	namespace px##__VA_ARGS__ {
#define PX_NAMESPACE_END()    	}

PX_NAMESPACE_BEGIN();
PX_NAMESPACE_END();

PX_NAMESPACE_BEGIN();

template<typename _Ty, size_t _Size>
class encstr
{
public:
    using seed_type = uint64_t;
    using char_type = _Ty;
    using char_pointer = char_type*;
    using string_view_type = std::basic_string_view<char_type>;
    using string_type = std::basic_string<char_type>;

    constexpr encstr() = default;

    constexpr encstr(const string_view_type& data, uint64_t seed = murmurhash3_64()) noexcept :
        m_Seed(seed)
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            m_String[i] = data[i] ^ static_cast<char_type>(seed >> ((i % 8) * 8));
        }
    }

    constexpr encstr(const char_type(&data)[_Size], uint64_t seed = murmurhash3_64()) noexcept :
        m_Seed(seed)
    {
        for (size_t i = 0; i < size(); i++)
        {
            m_String[i] = data[i] ^ static_cast<char_type>(seed >> ((i % 8) * 8));
        }
    }

    constexpr encstr(const std::array<char_type, _Size>& data, uint64_t seed = murmurhash3_64()) noexcept :
        m_Seed(seed)
    {
        for (size_t i = 0; i < size(); i++)
        {
            m_String[i] = data[i] ^ static_cast<char_type>(seed >> ((i % 8) * 8));
        }
    }

    constexpr encstr(const encstr& other) noexcept :
        m_Seed(other.seed())
    {
        for (size_t i = 0; i < size(); i++)
        {
            m_String[i] = other.data()[i] ^ static_cast<char_type>(seed() >> ((i % 8) * 8));
        }
    }

    constexpr encstr& operator=(const encstr& other) noexcept
    {
        m_Seed = other.seed();
        for (size_t i = 0; i < size(); i++)
        {
            m_String[i] = other.data()[i] ^ static_cast<char_type>(seed() >> ((i % 8) * 8));
        }
        return *this;
    }

    constexpr operator string_view_type() const noexcept
    {
        return string_view_type{ data(), size() - 1 };
    }

    _NODISCARD constexpr auto data() const noexcept
    {
        return m_String;
    }

    _NODISCARD constexpr auto str() const noexcept
    {
        return string_type{ data(), size() - 1 };
    }

    _NODISCARD constexpr size_t size() const noexcept
    {
        return _Size;
    }

    _NODISCARD constexpr seed_type seed() const noexcept
    {
        return m_Seed;
    }

private:
    _NODISCARD static constexpr uint64_t murmurhash3_64()
    {
        uint64_t seed = __COUNTER__;

        seed ^= (seed >> 33);
        seed *= 0xFF51'AFD7'ED55'8CCD;

        seed ^= (seed >> 33);
        seed *= 0xC4CE'B9FE'1A85'EC53;

        seed ^= (seed >> 33);
        return seed | 0x0101'0101'0101'0101;
    }

    seed_type m_Seed{ };
    char_type m_String[_Size]{ };
};

template<typename _Ty>
class encstr_ex
{
public:
    using seed_type = uint64_t;
    using char_type = _Ty;
    using char_pointer = char_type*;
    using string_view_type = std::basic_string_view<char_type>;
    using string_type = std::basic_string<char_type>;

    template<size_t _Size>
    constexpr encstr_ex(const encstr<char_type, _Size>& data) :
        m_Seed(data.seed())
    {
        (*this) += data;
    }

    template<size_t _Size>
    constexpr encstr_ex(const char_type(&data)[_Size], uint64_t seed = murmurhash3_64()) noexcept :
        m_Seed(seed)
    {
        (*this) += encstr(data);
    }

    template<size_t _Size>
    constexpr encstr_ex(const std::array<char_type, _Size>& data, uint64_t seed = murmurhash3_64()) noexcept :
        m_Seed(seed)
    {
        (*this) += encstr(data);
    }

    constexpr encstr_ex(const encstr_ex& data) :
        m_Seed(data.seed())
    {
        (*this) += data;
    }

    template<size_t _Size>
    encstr_ex<_Ty>& operator+=(const encstr<char_type, _Size>& data)
    {
        m_String.reserve(size() + data.size());
        for (size_t i = 0; i < data.size(); i++)
        {
            m_String.push_back(
                data.data()[i] ^ static_cast<char_type>(m_Seed >> ((i % 8) * 8))
            );
        }
        return *this;
    }

    encstr_ex<_Ty>& operator+=(const encstr_ex& data)
    {
        m_String.reserve(size() + data.size());
        for (size_t i = 0; i < data.size(); i++)
        {
            m_String.push_back(
                data.data()[i] ^ static_cast<char_type>(m_Seed >> ((i % 8) * 8))
            );
        }
        return *this;
    }

    template<size_t _Size>
    _NODISCARD encstr_ex<_Ty> operator+(const encstr<char_type, _Size>& data)
    {
        encstr_ex<_Ty> str(*this);
        return (str += data);
    }

    _NODISCARD encstr_ex<_Ty>& operator+(const encstr_ex& data)
    {
        encstr_ex<_Ty> str(*this);
        return (str += data);
    }

    constexpr operator const char_type* () const noexcept
    {
        return data();
    }

    constexpr operator string_view_type() const noexcept
    {
        return m_String;
    }

    _NODISCARD constexpr auto data() const noexcept
    {
        return m_String.data();
    }

    _NODISCARD constexpr const string_type& str() const noexcept
    {
        return m_String;
    }

    _NODISCARD string_type& str() noexcept
    {
        return m_String;
    }

    _NODISCARD constexpr size_t size() const noexcept
    {
        return m_String.size();
    }

    _NODISCARD constexpr seed_type seed() const noexcept
    {
        return m_Seed;
    }

private:
    static constexpr uint64_t murmurhash3_64()
    {
        uint64_t seed = __COUNTER__;

        seed ^= (seed >> 33);
        seed *= 0xFF51'AFD7'ED55'8CCD;

        seed ^= (seed >> 33);
        seed *= 0xC4CE'B9FE'1A85'EC53;

        seed ^= (seed >> 33);
        return seed | 0x0101'0101'0101'0101;
    }

    seed_type m_Seed{ };
    string_type m_String;
};

template<typename _Ty, size_t _Size>
_NODISCARD consteval auto make_encstr(encstr<_Ty, _Size>&& estr) noexcept
{
    return std::move(estr);
}

template<typename _Ty, size_t _Size>
_NODISCARD consteval auto make_encstr(const _Ty(&str)[_Size]) noexcept
{
    return encstr{ str };
}
template<typename _Ty, size_t _Size>
_NODISCARD consteval auto make_encstr(const std::array<_Ty, _Size>& str) noexcept
{
    return encstr{ str };
}

template<typename _Ty, size_t _Size>
_NODISCARD consteval auto make_str(const _Ty(&str)[_Size]) noexcept
{
    return make_encstr(make_encstr(str));
}
template<typename _Ty, size_t _Size>
_NODISCARD consteval auto make_str(const std::array<_Ty, _Size>& str) noexcept
{
    return make_encstr(make_encstr(str));
}

template<size_t _Size> using enc_str    = encstr<char, _Size>;
template<size_t _Size> using enc_wstr   = encstr<wchar_t, _Size>;
template<size_t _Size> using enc_u8str  = encstr<char8_t, _Size>;
template<size_t _Size> using enc_u16str = encstr<char16_t, _Size>;
template<size_t _Size> using enc_u32str = encstr<char32_t, _Size>;

using enc_strex     = encstr_ex<char>;
using enc_wstrex    = encstr_ex<wchar_t>;
using enc_u8strex   = encstr_ex<char8_t>;
using enc_u16strex  = encstr_ex<char16_t>;
using enc_u32strex  = encstr_ex<char32_t>;

using enc_strview       = enc_strex::string_view_type;
using enc_wstrview      = enc_wstrex::string_view_type;
using enc_u8strview     = enc_u8strex::string_view_type;
using enc_u16strview    = enc_u16strex::string_view_type;
using enc_u32strview    = enc_u32strex::string_view_type;

PX_NAMESPACE_END();