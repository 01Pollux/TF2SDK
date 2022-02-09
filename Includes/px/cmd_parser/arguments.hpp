#pragma once

#include <map>
#include "value.hpp"

namespace px
{
    template<typename _CmdArgTrait_t>
#ifdef _HAS_CXX23
        requires is_cmd_parser_traits_v<_CmdArgTrait_t>
#endif
    class [[nodiscard]] cmd_arg_t
    {
    public:
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;
        using value_t       = cmd_valinfo_t<traits_type>;

        using longkeymap_t = std::map<
            string_t,
            value_t
        >;

        static constexpr auto invalid_arg_v = char_t{ };

        cmd_arg_t() = default;

        cmd_arg_t(longkeymap_t&& other_long) :
            m_LongMap(std::move(other_long))
        {}

        cmd_arg_t(std::initializer_list<typename longkeymap_t::value_type> other_long) :
            m_LongMap(other_long)
        {}

        cmd_arg_t(std::initializer_list<std::tuple<string_t, char_t, value_t>> list)
        {
            for (auto& entry : list)
            {
                add_option(std::move(std::get<0>(entry)), std::move(std::get<2>(entry)), std::get<1>(entry));
            }
        }


        [[nodiscard]] bool contains(const string_t& name) const
        {
            return m_LongMap.contains(name);
        }

        [[nodiscard]] bool contains(char_t arg) const noexcept
        {
            return contains(string_t{ arg });
        }


        [[nodiscard]] bool empty() const noexcept
        {
            return m_LongMap.empty();
        }


        [[nodiscard]] auto find(const string_t& name) const
        {
            return m_LongMap.find(name);
        }
        
        [[nodiscard]] auto find(const string_t& name)
        {
            return m_LongMap.find(name);
        }

        [[nodiscard]] auto find(char_t arg) const
        {
            return find(string_t{ arg });
        }

        [[nodiscard]] auto find(char_t arg)
        {
            return find(string_t{ arg });
        }


        template<typename _Ty>
        [[nodiscard]] auto get(const string_t& name, const _Ty& default_type = { }) const
        {
            auto iter = find(name);
            return iter == end() ? default_type : iter->second.get_as<_Ty>();
        }

        template<typename _Ty>
        [[nodiscard]] auto get(char_t arg, const _Ty& default_type = { }) const
        {
            return get<_Ty>(string_t{ arg });
        }


        [[nodiscard]] auto& operator[](const string_t& name)
        {
            return m_LongMap[name];
        }

        [[nodiscard]] auto& operator[](char_t arg)
        {
            return (*this)[string_t{ arg }];
        }

    public:
        [[nodiscard]] auto begin() const noexcept
        {
            return m_LongMap.begin();
        }

        [[nodiscard]] auto end() const noexcept
        {
            return m_LongMap.end();
        }

        [[nodiscard]] auto rbegin() const noexcept
        {
            return m_LongMap.rbegin();
        }

        [[nodiscard]] auto rend() const noexcept
        {
            return m_LongMap.rend();
        }

        [[nodiscard]] auto begin() noexcept
        {
            return m_LongMap.begin();
        }

        [[nodiscard]] auto end() noexcept
        {
            return m_LongMap.end();
        }

        [[nodiscard]] auto rbegin() noexcept
        {
            return m_LongMap.rbegin();
        }

        [[nodiscard]] auto rend() noexcept
        {
            return m_LongMap.rend();
        }

        void add_option(string_view_t name, value_t&& value, char_t short_name = invalid_arg_v)
        {
            if (!name.empty())
            {
                auto res = m_LongMap.emplace(name, std::move(value));
                if (res.second && short_name == invalid_arg_v)
                {
                    m_LongMap.emplace(string_t{ short_name }, res.first->second);
                }
            }
            else m_LongMap.emplace(string_t{ short_name }, std::move(value));
        }
        
        void add_option(string_view_t name, const value_t& value, char_t short_name = invalid_arg_v)
        {
            if (!name.empty())
            {
                auto res = m_LongMap.emplace(name, value);
                if (res.second && short_name == invalid_arg_v)
                {
                    m_LongMap.emplace(string_t{ short_name }, res.first->second);
                }
            }
            else m_LongMap.emplace(string_t{ short_name }, value);
        }

        void erase_option(const string_t& name, char_t short_arg = invalid_arg_v) noexcept
        {
            if (!name.empty())
                m_LongMap.erase(name);

            if (short_arg != invalid_arg_v)
            {
                string_t short_arg_str{ short_arg };
                m_LongMap.erase(short_arg_str);
            }
        }

    private:
        longkeymap_t m_LongMap;
    };

    using cmd_arg     = cmd_arg_t<cmd_parser_default_traits>;
    using cmd_u8arg   = cmd_arg_t<cmd_parser_default_u8traits>;
    using cmd_warg    = cmd_arg_t<cmd_parser_default_wtraits>;
    using cmd_u16arg  = cmd_arg_t<cmd_parser_default_u16traits>;
    using cmd_u32arg  = cmd_arg_t<cmd_parser_default_u32traits>;

    template<typename _CmdArgTrait_t>
#ifdef _HAS_CXX23
        requires is_cmd_parser_traits_v<_CmdArgTrait_t>
#endif
    struct cmd_mask_t
    {
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;

        static constexpr auto invalid_arg_v = char_t{ };

        // long name of the argument
        string_view_t long_key;
        // short name of the argument
        char_t short_key = invalid_arg_v;
        //// help string
        //string_view_t help_string{ };

        // throws a runtime error if the key didnt exists in cmd_parser_t::parse
        bool is_required : 1 { };
        // only insert an empty string if the key exists
        bool no_value : 1{ };
        //bool is_vector : 1 { };
    };

    using cmd_mask      = cmd_mask_t<cmd_parser_default_traits>;
    using cmd_u8mask    = cmd_mask_t<cmd_parser_default_u8traits>;
    using cmd_wmask     = cmd_mask_t<cmd_parser_default_wtraits>;
    using cmd_u16mask   = cmd_mask_t<cmd_parser_default_u16traits>;
    using cmd_u32mask   = cmd_mask_t<cmd_parser_default_u32traits>;
}
