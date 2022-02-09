#pragma once

#include <map>
#include "traits.hpp"

namespace px
{
    template<typename _CmdArgTrait_t>
#ifdef _HAS_CXX23
        requires is_cmd_parser_traits_v<_CmdArgTrait_t>
#endif
    class cmd_bounded_t
    {
    public:
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;

        using value_t       = string_t;

        using reference_list = std::map<
            string_t,
            value_t
        >;

        bool exists(const string_view_t& name)
        {
            return m_Refs.contains(name);
        }

        bool bind(string_view_t name, const value_t& commands)
        {
            return m_Refs.emplace(name, commands).second;
        }

        bool bind(string_view_t name, value_t&& commands)
        {
            return m_Refs.emplace(name, std::move(commands)).second;
        }

        bool unbind(string_view_t name)
        {
            return m_Refs.erase(name);
        }

        const value_t& lookup(string_view_t name) const
        {
            auto iter = m_Refs.find(name);
            return iter == end() ? value_t{ '\0' } : name;
        }

        void clear() noexcept
        {
            m_Refs.clear();
        }

    public:
        [[nodiscard]] auto begin() const noexcept
        {
            return m_Refs.begin();
        }

        [[nodiscard]] auto end() const noexcept
        {
            return m_Refs.end();
        }

        [[nodiscard]] auto rbegin() const noexcept
        {
            return m_Refs.rbegin();
        }

        [[nodiscard]] auto rend() const noexcept
        {
            return m_Refs.rend();
        }

        [[nodiscard]] auto begin() noexcept
        {
            return m_Refs.begin();
        }

        [[nodiscard]] auto end() noexcept
        {
            return m_Refs.end();
        }

        [[nodiscard]] auto rbegin() noexcept
        {
            return m_Refs.rbegin();
        }

        [[nodiscard]] auto rend() noexcept
        {
            return m_Refs.rend();
        }

    private:
        reference_list m_Refs;
    };
    
    using cmd_bounded   = cmd_bounded_t<cmd_parser_default_traits>;
    using cmd_u8bounded = cmd_bounded_t<cmd_parser_default_u8traits>;
    using cmd_wbounded  = cmd_bounded_t<cmd_parser_default_wtraits>;
    using cmd_u16bounded= cmd_bounded_t<cmd_parser_default_u16traits>;
    using cmd_u32bounded= cmd_bounded_t<cmd_parser_default_u32traits>;
}
