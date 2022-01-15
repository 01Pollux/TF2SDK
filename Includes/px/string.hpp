#pragma once

#include <utility>
#include <algorithm>
#include <string>

namespace px
{
#if defined UNICODE
    using tchar = wchar_t;
#else
    using tchar = char;
#endif
    using tstring = std::basic_string<tchar>;
    using tstring_view = std::basic_string_view<tchar>;
    using tcstring = tchar*;

    template<typename _DstTy, typename _SrcTy>
    [[nodiscard]] constexpr auto string_cast(const _SrcTy& str)
    {
        if constexpr (std::is_same_v<_DstTy, _SrcTy>)
            return str;
        else if constexpr (std::is_pointer_v<_SrcTy>)
            return string_cast<_DstTy>(std::basic_string_view<std::remove_pointer_t<_SrcTy>>(str));
        else
        {
            if (str.empty())
                return _DstTy{ };
            else
            {
                // from bigger type to smaller type, eg: wstring to string, u32string to u8string, etc...
                if constexpr (sizeof(_SrcTy::value_type) > sizeof(_DstTy::value_type))
                {
                    _DstTy buf(str.size(), 0);
                    std::transform(std::begin(str), std::end(str), std::begin(buf), [](const _SrcTy::value_type c) { return static_cast<_DstTy::value_type>(c); });
                    return buf;
                }
                // from smaller type to bigger type, eg: string to wstring, etc...
                else return _DstTy{ std::begin(str), std::end(str) };
            }
        }
    }

    template<typename _DstTy, typename _SrcTy, size_t _Size>
    [[nodiscard]] constexpr auto string_cast(const std::array<_SrcTy, _Size>& str)
    {
        if constexpr (std::is_same_v<_DstTy, _SrcTy>)
            return str;

        else if constexpr (_Size < 1)
            return { };
        else
        {
            std::array<_DstTy, _Size> out_str{ };
            for (size_t i = 0; i < _Size; i++)
            {
                out_str[i] = static_cast<_SrcTy>(str[i]);
            }
            return out_str;
        }
    }

    template<typename _DstTy, typename _SrcTy, size_t _Size>
    [[nodiscard]] constexpr auto string_cast(const _SrcTy(&str)[_Size])
    {
        if constexpr (std::is_same_v<_DstTy, _SrcTy>)
            return str;

        else if constexpr (_Size < 1)
            return { };
        else
        {
            std::array<_DstTy, _Size> out_str{ };
            for (size_t i = 0; i < _Size; i++)
            {
                out_str[i] = static_cast<_DstTy>(str[i]);
            }
            return out_str;
        }
    }
}