#pragma once

#include "parser.hpp"

namespace px
{
    template<typename _CmdArgTrait_t>
#ifdef _HAS_CXX23
        requires is_cmd_parser_traits_v<_CmdArgTrait_t>
#endif
    class cmd_valinfo_t
    {
    public:
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;

        constexpr cmd_valinfo_t(string_t&& val)
            noexcept (std::is_nothrow_move_constructible_v<string_t>) :
            m_Value(std::move(val))
        {}

        constexpr cmd_valinfo_t(string_view_t val = { })
            noexcept (std::is_nothrow_constructible_v<string_t>) :
            m_Value(val)
        {}

        constexpr cmd_valinfo_t(const char_t* val)
            noexcept (std::is_nothrow_constructible_v<string_t>) :
            m_Value(val)
        {}

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return m_Value.empty();
        }

        [[nodiscard]] constexpr const string_t& str() const noexcept
        {
            return m_Value;
        }
        
        [[nodiscard]] constexpr string_t& str() noexcept
        {
            return m_Value;
        }
        
        template<typename _Ty>
        [[nodiscard]] _Ty get_as(string_view_t split_str = traits_type::default_split_str) const noexcept
        {
            return cmd_arg_info<_Ty>::template from_string<char_t>(string_view_t(str()), split_str);
        }

        template<typename _Ty>
        [[nodiscard]] auto split(string_view_t split_str = traits_type::default_split_str) const noexcept
        {
            return get_as<std::vector<_Ty>>(split_str);
        }

    private:
        string_t m_Value;
    };

    using cmd_valinfo       = cmd_valinfo_t<cmd_parser_default_traits>;
    using cmd_u8valinfo     = cmd_valinfo_t<cmd_parser_default_u8traits>;
    using cmd_wvalinfo      = cmd_valinfo_t<cmd_parser_default_wtraits>;
    using cmd_u16valinfo    = cmd_valinfo_t<cmd_parser_default_u16traits>;
    using cmd_u32valinfo    = cmd_valinfo_t<cmd_parser_default_u32traits>;
}
