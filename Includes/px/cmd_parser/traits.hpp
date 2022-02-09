#pragma once

#include <type_traits>
#include <string>

namespace px
{
    template<typename _Ty>
    static constexpr bool is_cmd_parser_traits_v = false;

#define PX_CMD_PARSER_DEFINE_TRAITS(Name, Char, DefaultSplit)            \
    struct Name final                                                    \
    {                                                                    \
        using char_t        = Char;                                      \
        using string_t      = std::basic_string<Char>;                   \
        using string_view_t = std::basic_string_view<Char>;              \
                                                                         \
        static constexpr string_view_t default_split_str = DefaultSplit; \
    };                                                                   \
    template<>                                                           \
    static constexpr bool is_cmd_parser_traits_v<Name> = true

    PX_CMD_PARSER_DEFINE_TRAITS(cmd_parser_default_traits,      char,    ";-;");
    PX_CMD_PARSER_DEFINE_TRAITS(cmd_parser_default_u8traits,    char8_t, u8";-;");
    PX_CMD_PARSER_DEFINE_TRAITS(cmd_parser_default_wtraits,     wchar_t, L";-;");
    PX_CMD_PARSER_DEFINE_TRAITS(cmd_parser_default_u16traits,   char16_t, u";-;");
    PX_CMD_PARSER_DEFINE_TRAITS(cmd_parser_default_u32traits,   char32_t, U";-;");

    template<typename _Ty>
    static constexpr bool is_std_string_view_v =
        std::is_same_v<_Ty, std::string_view>   ||
        std::is_same_v<_Ty, std::u8string_view> ||
        std::is_same_v<_Ty, std::wstring_view>  ||
        std::is_same_v<_Ty, std::u16string_view>||
        std::is_same_v<_Ty, std::u32string_view>;

    template<typename _Ty>
    static constexpr bool is_std_string_v =
        std::is_same_v<_Ty, std::string>   ||
        std::is_same_v<_Ty, std::u8string> ||
        std::is_same_v<_Ty, std::wstring>  ||
        std::is_same_v<_Ty, std::u16string>||
        std::is_same_v<_Ty, std::u32string>;
}
