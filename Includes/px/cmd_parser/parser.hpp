#pragma once

#include <charconv>
#include <algorithm>
#include <vector>
#include <array>

#if __cpp_lib_ranges
#include <ranges>
#else
#include <boost/algorithm/string.hpp>
#endif

#include "traits.hpp"

namespace px
{
    template<typename _Ty>
    struct cmd_arg_info
    {
        struct typeinfo
        {
            static constexpr const char* name = "???";
            static constexpr bool has_lexial_cast = false;
        };

        template<typename _CharTy>
        [[nodiscard]] static auto from_string(
            std::basic_string_view<_CharTy> value,
            std::basic_string_view<_CharTy> delimiters
        ) noexcept
        {
            try
            {
                if constexpr (typeinfo::has_lexial_cast)
                {
                    if constexpr (std::is_same_v<_Ty, bool>)
                    {
                        switch (value[0])
                        {
                        case '\0':
                        case '0':
                            return false;
                        case '1':
                        {
                            return true;
                        }
                        default:
                        {
                            return !value.compare("true");
                        }
                        }
                    }
                    else return to_string_impl(value);
                }
                else if constexpr (requires(_Ty& v) { v.emplace_back(std::basic_string_view<_CharTy>{ }); })
                {
                    _Ty res;
#if __cpp_lib_ranges
                    const auto tokens = value | std::views::split(delimiters) | std::views::transform([](auto&& rng)
						{
							return std::string_view(&*rng.begin(), std::ranges::distance(rng));
						});
                    res.reserve(std::ranges::distance(tokens));
#else
                    std::vector<std::basic_string_view<_CharTy>> tokens;
                    boost::split(tokens, value, boost::is_any_of(delimiters));
                    res.reserve(tokens.size());
#endif
                    for (const auto& tok : tokens)
                    {
                        if constexpr (is_std_string_view_v<_Ty::value_type> || is_std_string_v<_Ty::value_type>)
                        {
                            res.emplace_back(tok);
                        }
                        else
                        {
                            res.emplace_back(
                                cmd_arg_info<_Ty::value_type>::from_string(
                                    tok,
                                    delimiters
                                )
                            );
                        }

                    }

                    return res;
                }
                else if constexpr (requires(_Ty& v) { v.begin(); })
                {
                    _Ty res;
                    const auto iter = res.begin();
                    const auto end = res.begin();

#if __cpp_lib_ranges
                    auto tokens = value | std::views::split(delimiters) | std::views::transform([](auto&& rng)
                        {
                            return std::string_view(&*rng.begin(), std::ranges::distance(rng));
                        });
#else
                    std::vector<std::basic_string_view<_CharTy>> tokens;
                    boost::split(tokens, value, boost::is_any_of(delimiters));
#endif

                    for (const auto& tok : tokens)
                    {
                        if constexpr (is_std_string_view_v<_Ty::value_type> || is_std_string_v<_Ty::value_type>)
                        {
                            *iter++ = tok;
                        }
                        else
                        {
                            *iter++ = cmd_arg_info<_Ty::value_type>::from_string(
                                tok,
                                delimiters
                            );
                        }
                        if (iter == end)
                            break;
                    }

                    return res;
                }
                else
                {
                    return _Ty::from_str(value);
                }
            }
            catch (...)
            {
                return _Ty{ };
            }
        }

        template<typename _CharTy>
        [[nodiscard]] static std::basic_string<_CharTy> to_string(
            std::basic_string_view<_CharTy> value,
            std::basic_string_view<_CharTy> delimiter
        ) noexcept
        {
            try
            {
                if constexpr (typeinfo::has_lexial_cast)
                {
                    return to_string_impl(value);
                }
                else if constexpr (requires(const _Ty& v) { v.begin(); })
                {
                    std::basic_string<_CharTy> res;

                    const auto iter = value.begin();
                    const auto end = value.end();

                    if (iter != end)
                    {
                        --end;

                        for (; iter != end; iter++)
                        {
                            if constexpr (is_std_string_view_v<_Ty::value_type> || is_std_string_v<_Ty::value_type>)
                                res += *iter;
                            else res += cmd_arg_info<_Ty::value_type>::to_string(*iter, delimiter);
                            res += delimiter;
                        }

                        res += cmd_arg_info<_Ty::value_type>::to_string(*iter);
                    }
                   
                    return res;
                }
                else
                {
                    return _Ty::to_str(value);
                }
            }
            catch (...)
            {
                return { };
            }
        }

    private:
        template<typename _CharTy>
        static _Ty to_string_impl(std::basic_string_view<_CharTy> value)
        {
            _Ty val{ };
            if constexpr (std::is_same_v<_CharTy, char>)
            {
                std::from_chars(value.data(), value.data() + value.size(), val);
            }
            else
            {
                _Ty val{ };
                std::basic_string<char> buf(value.size(), 0);
                std::transform(std::begin(value), std::end(value), std::begin(buf), [](const _CharTy c) { return static_cast<char>(c); });
                std::from_chars(buf.data(), buf.data() + buf.size(), val);
            }
            return val;
        }
    };
}

#define PX_CMD_TYPEINFO_ARR(Name, Type, Size)                       \
template<>												            \
struct px::cmd_arg_info<std::array<Type, Size>>::typeinfo	        \
{																	\
	static constexpr const char* name = Name##"["###Size##"]";      \
    static constexpr bool has_lexial_cast = false;                  \
}

#define PX_CMD_TYPEINFO_VEC(Name, Type)                     \
template<>												    \
struct px::cmd_arg_info<std::vector<Type>>::typeinfo        \
{															\
	static constexpr const char* name = Name##"[]";         \
    static constexpr bool has_lexial_cast = false;          \
}



#define PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, Size)                                 \
template<>												                                \
struct px::cmd_arg_info<std::array<Type, Size>>::typeinfo                               \
{                                                                                       \
    using parent_type = px::cmd_arg_info<std::array<Parent, Size>>::typeinfo;           \
    static constexpr const char* name = parent_type::name;	                            \
    static constexpr bool has_lexial_cast = parent_type::has_lexial_cast;               \
}

#define PX_CMD_TYPEINFO_VEC_INHERIT(Type, Parent)                               \
template<>												                        \
struct px::cmd_arg_info<std::array<Type>>::typeinfo                             \
{                                                                               \
    using parent_type = px::cmd_arg_info<std::vector<Parent>>::typeinfo;        \
    static constexpr const char* name = parent_type::name;	                    \
    static constexpr bool has_lexial_cast = parent_type::has_lexial_cast;       \
}

#define PX_CMD_TYPEINFO(Name, Type)					\
template<>											\
struct px::cmd_arg_info<Type>::typeinfo		        \
{													\
	static constexpr const char* name = Name;	    \
    static constexpr bool has_lexial_cast = true;   \
};													\
PX_CMD_TYPEINFO_ARR(Name, Type, 1);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 2);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 3);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 4);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 5);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 6);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 7);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 8);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 9);                 \
PX_CMD_TYPEINFO_ARR(Name, Type, 10);                \
PX_CMD_TYPEINFO_ARR(Name, Type, 11);                \
PX_CMD_TYPEINFO_ARR(Name, Type, 12);                \
PX_CMD_TYPEINFO_VEC(Name, Type)


#define PX_CMD_TYPEINFO_INHERIT(Type, Parent)                               \
template<>                                                                  \
struct px::cmd_arg_info<Type>::typeinfo                                     \
{                                                                           \
    using parent_type = px::cmd_arg_info<Parent>::typeinfo;                 \
    static constexpr const char* name = parent_type::name;	                \
    static constexpr bool has_lexial_cast = parent_type::has_lexial_cast;   \
};                                                                          \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 1);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 2);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 3);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 4);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 5);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 6);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 7);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 8);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 9);                               \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 10);                              \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 11);                              \
PX_CMD_TYPEINFO_ARR_INHERIT(Type, Parent, 12);                              \
PX_CMD_TYPEINFO_VEC_INHERIT(Name, Type)


PX_CMD_TYPEINFO("int",		int);
PX_CMD_TYPEINFO("long",		long);
PX_CMD_TYPEINFO("llong",	long long);

PX_CMD_TYPEINFO("uint",     unsigned int);
PX_CMD_TYPEINFO("ulong",    unsigned long);
PX_CMD_TYPEINFO("ullong",	unsigned long long);

PX_CMD_TYPEINFO("bool",		bool);
PX_CMD_TYPEINFO("char",		char);
PX_CMD_TYPEINFO("char8",	char8_t);
PX_CMD_TYPEINFO("wchar",	wchar_t);
PX_CMD_TYPEINFO("char16",	char16_t);
PX_CMD_TYPEINFO("char32",	char32_t);
PX_CMD_TYPEINFO("short",	short);

PX_CMD_TYPEINFO("uchar",	unsigned char);
PX_CMD_TYPEINFO("ushort",	unsigned short);

PX_CMD_TYPEINFO("float",	float);
PX_CMD_TYPEINFO("double",	double);
PX_CMD_TYPEINFO("ldouble",	long double);

PX_CMD_TYPEINFO("string",	std::string);
PX_CMD_TYPEINFO("string8",	std::u8string);
PX_CMD_TYPEINFO("wstring",	std::wstring);
PX_CMD_TYPEINFO("string16",	std::u16string);
PX_CMD_TYPEINFO("string32",	std::u32string);
