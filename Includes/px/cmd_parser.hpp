#pragma once

#include <stdexcept>
#include "cmd_parser/commands.hpp"

namespace px
{
    template<typename _CmdArgTrait_t>
#ifdef _HAS_CXX23
        requires is_cmd_parser_traits_v<_CmdArgTrait_t>
#endif
    class cmd_parser_t
    {
    public:
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;

        using exec_type     = con_exec_info_t<traits_type>;
        using mask_type     = cmd_mask_t<traits_type>;

        /// <summary>
        /// mask and parse a static/dynamic array of strings to a command's parameters
        /// </summary>
        template<typename _MaskIter>
        [[nodiscard]] static exec_type parse(const std::vector<string_view_t>& argv, _MaskIter mask_begin, _MaskIter mask_end)
        {
            cmd_arg_t<traits_type> info;

            auto iter = argv.begin(), end = argv.end();
            while (iter != end)
            {
                if (iter->size() && (*iter)[0] == '-')
                {
                    bool many_flags = iter->size() > 1 && (*iter)[1] != '-';
                    string_view_t arg_name = many_flags ? string_view_t{ iter->begin() + 1, iter->end() } : string_view_t{ iter->begin() + 2, iter->end() };
                    bool do_continue = true;
                    
                    for (auto& mask = mask_begin; mask != mask_end; ++mask)
                    {
                        if (many_flags)
                        {
                            if (arg_name.find(mask->short_key) == arg_name.npos)
                                continue;
                        }
                        else
                        {
                            if (arg_name.compare(mask->long_key))
                                continue;
                        }

                        do_continue = false;
                        if (++iter == end)
                        {
                            info.add_option(
                                mask->long_key,
                                string_view_t{ },
                                mask->short_key
                            );
                            break;
                        }
                        else
                        {
                            info.add_option(
                                mask->long_key,
                                mask->no_value ? string_view_t{} : string_view_t{ iter->begin(), iter->end() },
                                mask->short_key
                            );
                            if (!mask->no_value)
                                ++iter;
                        }
                        
                        break;
                    }

                    if (do_continue)
                        ++iter;
                }
                else break;
            }

            string_t value;
            while (iter != end)
            {
                value += string_t{ iter->begin(), iter->end() };
                if (++iter != end)
                    value += char_t{ ' ' };
            }

            for (auto& mask = mask_begin; mask != mask_end; ++mask)
            {
                if (mask->is_required && (
                        (!mask->long_key.empty() && !info.contains(string_t{ mask->long_key })) ||
                        (mask->short_key != mask->invalid_arg_v && !info.contains(mask->short_key))
                   ))
                    throw std::runtime_error("missing arguments token(s).");
            }

            return { std::move(info), std::move(value)};
        }

        /// <summary>
        /// mask and parse a an array of char pointers to a command's parameters
        /// </summary>
        [[nodiscard]] static exec_type parse(char_t** argv, size_t argc, auto mask_begin, auto mask_end)
        {
            assert(argv != nullptr);
            std::vector<string_view_t> args_value(argv + 1, argv + argc);
            return parse(args_value, mask_begin, mask_end);
        }

        /// <summary>
        /// mask and parse a string to a command's parameters
        /// </summary>
        template<typename _MaskGetFn>
        [[nodiscard]] static std::vector<exec_type> parse(string_view_t full_cmd, const _MaskGetFn& get_cmd_mask)
        {
            std::vector<exec_type> infos;
            {
                std::vector strs = split_string_to_cmds(full_cmd);
                infos.reserve(strs.size());

                typename std::vector<mask_type>::const_iterator mask_begin, mask_end;

                for (auto str : strs)
                {
                    auto iter = str.begin(), end = str.end();
                    while (iter != end && !std::isspace(*iter))
                        ++iter;

                    bool should_handle = get_cmd_mask(string_view_t{ str.begin(), iter }, mask_begin, mask_end);
                    
                    if (should_handle)
                    {
                        if (iter != str.end())
                        {
                            std::vector argv = split_string_to_argv(string_view_t{ ++iter, str.end() });
                            infos.emplace_back(parse(argv, mask_begin, mask_end));
                            continue;
                        }
                    }

                    infos.emplace_back();
                }
            }
            return infos;
        }

    private:
        [[nodiscard]] static std::vector<string_view_t> split_string_to_cmds(string_view_t str)
        {
            auto iter = str.begin(), end = str.end(), last_begin = iter;
            std::vector<std::string_view> strs;

            while (iter != end)
            {
                while (iter != end && (std::isspace(*iter) || *iter == ';'))
                    ++last_begin, ++iter;

                if (iter == end)
                    break;

                enum class ParseState : char
                {
                    None,
                    SkipNext,
                    InQuote,
                    EndOfCmd
                } state{  };

                while (iter != end)
                {
                    if (state == ParseState::SkipNext)
                    {
                        state = ParseState::None;
                        ++iter;
                        continue;
                    }

                    switch (*iter)
                    {
                    case '\\':
                    {
                        state = ParseState::SkipNext;
                        break;
                    }
                    case '"':
                    {
                        state = state == ParseState::InQuote ? ParseState::None : ParseState::InQuote;
                        break;
                    }
                    case ';':
                    {
                        if (state != ParseState::InQuote)
                            state = ParseState::EndOfCmd;
                        break;
                    }
                    }

                    if (++iter == end || state == ParseState::EndOfCmd)
                    {
                        auto last_end = iter - 1;
                        while (last_end != last_begin && *last_end == ';')
                            --last_end;
                        strs.emplace_back(last_begin, last_end + 1);

                        if (iter != end)
                            last_begin = iter;
                        break;
                    }
                }
            }

            return strs;
        }

        [[nodiscard]] static std::vector<string_view_t> split_string_to_argv(string_view_t str)
        {
            auto iter = str.begin(), end = str.end(), last_begin = iter;
            std::vector<string_view_t> strs;
          
            while (iter != end)
            {
                while (iter != end && std::isspace(*iter))
                    ++iter;

                last_begin = iter;

                enum class ParseState : char
                {
                    None,
                    SkipNext,
                    InQuote,
                    Break
                } state{ };

                while (iter != end && state != ParseState::Break)
                {
                    if (state == ParseState::SkipNext)
                    {
                        state = ParseState::None;
                        ++iter;
                        continue;
                    }

                    switch (*iter)
                    {
                    case '\\':
                    {
                        state = ParseState::SkipNext;
                        break;
                    }
                    case '"':
                    {
                        if (state == ParseState::None)
                        {
                            state = ParseState::InQuote;
                        }
                        else
                        {
                            strs.emplace_back(last_begin, iter);
                            state = ParseState::Break;
                        }
                        last_begin = iter + 1;
                        break;
                    }
                    default:
                    {
                        if (state != ParseState::InQuote && std::isspace(*iter))
                        {
                            strs.emplace_back(last_begin, iter);
                            last_begin = iter + 1;
                            state = ParseState::Break;
                        }
                        break;
                    }
                    }
                    ++iter;
                }
            }

            if (last_begin != iter)
                strs.emplace_back(last_begin, iter);
            return strs;
        }
    };

    
    using cmd_parser    = cmd_parser_t<cmd_parser_default_traits>;
    using cmd_u8parser  = cmd_parser_t<cmd_parser_default_u8traits>;
    using cmd_wparser   = cmd_parser_t<cmd_parser_default_wtraits>;
    using cmd_u16parser = cmd_parser_t<cmd_parser_default_u16traits>;
    using cmd_u32parser = cmd_parser_t<cmd_parser_default_u32traits>;
}
