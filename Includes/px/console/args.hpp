#pragma once

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <px/defines.hpp>

PX_NAMESPACE_BEGIN();

template<typename _Ty>
struct CommandParser
{
public:
    struct typeinfo
    {
        static constexpr const char* type_name = "???";
        static constexpr bool has_lexial_cast = false;
    };

    [[nodiscard]] static auto from_string(const std::string_view& value, const char* delimiters = "[{,}]")
    {
        if constexpr (typeinfo::has_lexial_cast)
        {
            return boost::lexical_cast<_Ty>(value);
        }
        else if constexpr (requires(_Ty& v) { v.emplace_back(); })
        {
            using tokenizer = boost::tokenizer<boost::char_separator<char>, std::string_view::const_iterator>;

            tokenizer tokens{ value, boost::char_separator<char>{delimiters} };

            _Ty res;
            res.reserve(std::distance(tokens.begin(), tokens.end()));

            for (auto& tok : tokens)
                res.emplace_back(CommandParser<_Ty::value_type>::from_string(tok));

            return res;
        }
        else if constexpr (requires(_Ty& v) { v.begin(); })
        {
            using tokenizer = boost::tokenizer<boost::char_separator<char>, std::string_view::const_iterator>;

            _Ty res;
            auto iter = res.begin();

            for (auto& tok : tokenizer{ value, boost::char_separator<char>{delimiters} })
            {
                *iter = CommandParser<_Ty::value_type>::from_string(tok);
                ++iter;
            }
            return res;
        }
        else
        {
            return _Ty::from_str(value);
        }
    }

    [[nodiscard]] static std::string to_string(const _Ty& value, const char delimiters[3] = "{,}")
    {
        if constexpr (typeinfo::has_lexial_cast)
        {
            return boost::lexical_cast<std::string>(value);
        }
        else if constexpr (requires(const _Ty& v) { v.begin(); })
        {
            std::string res{ delimiters[0] };

            auto iter = value.begin();
            auto end = value.end() - 1;
            for (; iter != end; iter++)
                res += CommandParser<_Ty::value_type>::to_string(*iter) + delimiters[1];

            res += CommandParser<_Ty::value_type>::to_string(*iter) + delimiters[2];
            return res;
        }
        else
        {
            return _Ty::to_str(value);
        }
    }
};

class CommandArgs
{
public:
	CommandArgs(std::vector<std::pair<std::string_view, std::string_view>>&& argv, const std::string_view& value) noexcept :
		m_ArgV(std::move(argv)), m_Value(value) { }

	[[nodiscard]] bool contains(const std::string_view& arg_name) const noexcept
	{
		for (auto& arg_val : m_ArgV)
			if (arg_name == arg_val.first)
				return true;
		return false;
	}

    [[nodiscard]] bool contains(const std::string_view& arg_name)
	{
		for (auto& arg_val : m_ArgV)
		{
			if (arg_val.first == arg_name)
				return true;
		}
		return false;
	}

	template<typename _Ty = std::string_view>
    [[nodiscard]] decltype(auto) get_arg(
		const std::string_view& arg_name,
		const _Ty& default_value = { },
        const char* delimiters = "[{,}]"
	) const
	{
		for (auto& arg_val : m_ArgV)
		{
            if (arg_name == arg_val.first)
            {
                if constexpr (std::is_same_v<_Ty, std::string_view> || std::is_same_v<_Ty, std::string>)
                    return arg_val.second;
                else if constexpr (std::is_same_v<_Ty, const char*>)
                    return arg_val.second.data();
                else
                    return CommandParser<_Ty>::from_string(arg_val.second, delimiters);
            }
		}
		return _Ty(default_value);
	}

    [[nodiscard]] size_t arg_size() const noexcept
	{
		return m_ArgV.size();
	}

    [[nodiscard]] size_t has_args() const noexcept
	{
		return !m_ArgV.empty();
	}

	template<typename _Ty = std::string_view>
    [[nodiscard]] decltype(auto) get_val(
		const _Ty& default_value = { },
        const char* delimiters = "[{,}]"
	) const
	{
        if constexpr (std::is_same_v<_Ty, std::string_view> || std::is_same_v<_Ty, std::string>)
            return !has_val() ? default_value : m_Value;
        else if constexpr (std::is_same_v<_Ty, const char*>)
            return !has_val() ? default_value : m_Value.data();
        else
            return !has_val() ? default_value : CommandParser<_Ty>::from_string(m_Value, delimiters);
	}

    [[nodiscard]] size_t val_size() const noexcept
	{
		return m_Value.size();
	}

    [[nodiscard]] size_t has_val() const noexcept
	{
		return val_size() > 0;
	}

private:
	const std::vector<std::pair<std::string_view, std::string_view>> m_ArgV;
	const std::string_view m_Value;
};

PX_NAMESPACE_END();

#define PX_CONVAR_TYPEINFO_ARR(Name, Type, Size)                                            \
template<>												                                    \
struct px::CommandParser<std::array<Type, Size>>::typeinfo			                        \
{																	                        \
	static constexpr const char* type_name = Name##"["###Size##"]";                         \
    static constexpr bool has_lexial_cast = false;                                          \
}


#define PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, Size)                                  \
template<>												                                    \
struct px::CommandParser<std::array<Type, Size>>::typeinfo                                  \
{                                                                                           \
    using parent_type = px::CommandParser<std::array<Parent, Size>>::typeinfo;              \
    static constexpr const char* type_name = parent_type::type_name;	                    \
    static constexpr bool has_lexial_cast = parent_type::has_lexial_cast;                   \
}

#define PX_CONVAR_TYPEINFO(Name, Type)								    \
template<>															    \
struct px::CommandParser<Type>::typeinfo						        \
{																	    \
	static constexpr const char* type_name = Name;					    \
    static constexpr bool has_lexial_cast = true;                       \
};																	    \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 1);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 2);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 3);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 4);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 5);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 6);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 7);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 8);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 9);                                  \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 10);                                 \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 11);                                 \
PX_CONVAR_TYPEINFO_ARR(Name, Type, 12)


#define PX_CONVAR_TYPEINFO_INHERIT(Type, Parent)                            \
template<>                                                                  \
struct px::CommandParser<Type>::typeinfo                                    \
{                                                                           \
    using parent_type = px::CommandParser<Parent>::typeinfo;                \
    static constexpr const char* type_name = parent_type::type_name;	    \
    static constexpr bool has_lexial_cast = parent_type::has_lexial_cast;   \
};                                                                          \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 1);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 2);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 3);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 4);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 5);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 6);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 7);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 8);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 9);                            \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 10);                           \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 11);                           \
PX_CONVAR_TYPEINFO_ARR_INHERIT(Type, Parent, 12)


PX_CONVAR_TYPEINFO("int",		int);
PX_CONVAR_TYPEINFO("long",		long);
PX_CONVAR_TYPEINFO("llong",		long long);

PX_CONVAR_TYPEINFO("uint",      unsigned int);
PX_CONVAR_TYPEINFO("ulong",     unsigned long);
PX_CONVAR_TYPEINFO("ullong",	unsigned long long);

PX_CONVAR_TYPEINFO("bool",		bool);
PX_CONVAR_TYPEINFO("char",		char);
PX_CONVAR_TYPEINFO("char8",		char8_t);
PX_CONVAR_TYPEINFO("wchar",		wchar_t);
PX_CONVAR_TYPEINFO("char16",	char16_t);
PX_CONVAR_TYPEINFO("char32",	char32_t);
PX_CONVAR_TYPEINFO("short",		short);

PX_CONVAR_TYPEINFO("uchar",		unsigned char);
PX_CONVAR_TYPEINFO("ushort",	unsigned short);

PX_CONVAR_TYPEINFO("float",		float);
PX_CONVAR_TYPEINFO("double",	double);
PX_CONVAR_TYPEINFO("ldouble",	long double);

PX_CONVAR_TYPEINFO("string",	std::string);
PX_CONVAR_TYPEINFO("string8",	std::u8string);
PX_CONVAR_TYPEINFO("wstring",	std::wstring);
PX_CONVAR_TYPEINFO("string16",	std::u16string);
PX_CONVAR_TYPEINFO("string32",	std::u32string);