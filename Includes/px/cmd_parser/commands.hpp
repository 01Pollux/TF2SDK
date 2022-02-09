#pragma once

#include "arguments.hpp"

namespace px
{
    template<typename _CmdArgTrait_t>
    struct con_exec_info_t;

    template<typename _Ty>
    class cmd_manager_t;

    template<typename _CmdArgTrait_t>
#ifdef _HAS_CXX23
        requires is_cmd_parser_traits_v<_CmdArgTrait_t>
#endif
    class con_command_t
    {
        template<typename _Ty>
        friend class cmd_manager_t;

    public:
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;

        using entries_type  = std::map<string_t, con_command_t*>;
        using mask_type     = cmd_mask_t<traits_type>;

        using exec_info_type = con_exec_info_t<traits_type>;
        using invoke_proc = void (*)(con_command_t* command, const exec_info_type& exec_info);

        con_command_t(
            string_view_t name,
            invoke_proc callback,
            string_view_t description = { },
            std::vector<mask_type>&& masks = { }
        ) noexcept :
            m_Name(std::move(name)), m_Description(std::move(description)), m_Callback(callback), m_Masks(std::move(masks))
        {
            m_Commands.emplace(m_Name, this);
        }

        [[nodiscard]] const string_view_t& name() const noexcept { return m_Name; }
        [[nodiscard]] const string_view_t& help() const noexcept { return m_Description; }
        [[nodiscard]] auto& masks() const noexcept { return m_Masks; }

        [[nodiscard]] bool is_command() const noexcept
        {
            return m_IsCommand;
        }

        void operator()() noexcept(
            std::is_nothrow_invocable_v<invoke_proc, con_command_t*, const exec_info_type&>
        )
        {
            m_Callback(this, { });
        }
        void operator()(cmd_arg_t<traits_type>&& args) noexcept(
            std::is_nothrow_invocable_v<invoke_proc, con_command_t*, const exec_info_type&>
        )
        {
            m_Callback(this, exec_info_type{ std::move(args) });
        }
        void operator()(cmd_valinfo_t<traits_type>&& value) noexcept(
            std::is_nothrow_invocable_v<invoke_proc, con_command_t*, const exec_info_type&>
        )
        {
            m_Callback(this, exec_info_type{ std::move(value) });
        }
        void operator()(cmd_arg_t<traits_type>&& args, cmd_valinfo_t<traits_type>&& value) noexcept(
            std::is_nothrow_invocable_v<invoke_proc, con_command_t*, const exec_info_type&>
        )
        {
            m_Callback(this, exec_info_type{ std::move(args), std::move(value) });
        }
        void operator()(const exec_info_type& exec_info) noexcept(
            std::is_nothrow_invocable_v<invoke_proc, con_command_t*, const exec_info_type&>
        )
        {
            m_Callback(this, exec_info);
        }
        void operator()(exec_info_type&& exec_info) noexcept(
            std::is_nothrow_invocable_v<invoke_proc, con_command_t*, const exec_info_type&>
        )
        {
            m_Callback(this, std::move(exec_info));
        }

    protected:
        void set_as_cvar() noexcept
        {
            m_IsCommand = false;
        }

    private:
        string_view_t m_Name;
        string_view_t m_Description;

        std::vector<mask_type> m_Masks;
        invoke_proc m_Callback;

        bool m_IsCommand : 1{ true };
        static inline entries_type m_Commands;
    };

    template<typename _Ty, typename _CmdArgTrait_t>
#ifdef _HAS_CXX23
        requires is_cmd_parser_traits_v<_CmdArgTrait_t>
#endif
    class con_var_t : public con_command_t<_CmdArgTrait_t>
    {
    public:
        using traits_type   = _CmdArgTrait_t;
        using parent_type   = con_command_t<traits_type>;
        using exec_info_type= typename parent_type::exec_info_type;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;
        using mask_type     = typename parent_type::mask_type;
        
        using value_type    = _Ty;
        using reference     = value_type&;
        using const_reference= const value_type&;;
        using pointer       = value_type*;
        using const_pointer = const value_type*;

        static constexpr const char* type_name = cmd_arg_info<value_type>::typeinfo::name;

        constexpr con_var_t(
            string_view_t name,
            const_reference value = { }, 
            string_view_t description = {}, 
            std::vector<mask_type>&& masks = { },
            parent_type::invoke_proc callback = &default_command_cvar_set<>
        ) noexcept :
            parent_type(std::move(name), callback, std::move(description), std::move(masks)), m_Value(value)
        {
            parent_type::set_as_cvar();
        }

        constexpr con_var_t(
            string_view_t name, 
            value_type&& value, 
            string_view_t description = {}, 
            std::vector<mask_type>&& masks = { },
            parent_type::invoke_proc callback = &default_command_cvar_set<>
        ) noexcept :
            parent_type(std::move(name), callback, std::move(description), std::move(masks)), m_Value(value)
        {
            parent_type::set_as_cvar();
        }

        [[nodiscard]] constexpr const_reference get() const noexcept { return m_Value; }
        [[nodiscard]] constexpr reference get() noexcept             { return m_Value; }

        [[nodiscard]] constexpr operator const_reference() const noexcept { return get(); }
        [[nodiscard]] constexpr operator reference() noexcept           { return get(); }

        template<typename _OTy = value_type> constexpr auto& operator=(const con_var_t<_OTy, traits_type>& o) { m_Value = static_cast<const_reference>(o.m_Value); return *this; }
        template<typename _OTy = value_type> constexpr auto& operator=(const _OTy& o) { m_Value = static_cast<const_reference>(o); return *this; }
                                             
        template<typename _OTy = value_type> constexpr [[nodiscard]] auto operator<=>(const _OTy& o) const noexcept { return m_Value <=> static_cast<const_reference>(o); }
        template<typename _OTy = value_type> constexpr [[nodiscard]] bool operator!=(const _OTy& o) const noexcept { return m_Value != static_cast<const_reference>(o); }
                                             
        template<typename _OTy = value_type> constexpr [[nodiscard]] auto operator<=>(const con_var_t<_OTy, traits_type>& prop) const noexcept { return m_Value <=> static_cast<const_reference>(prop.m_Value); }
        template<typename _OTy = value_type> constexpr [[nodiscard]] bool operator!=(const con_var_t<_OTy, traits_type>& prop) const noexcept { return m_Value != static_cast<const_reference>(prop.m_Value); }

        constexpr auto& operator++() noexcept { ++m_Value; return *this; }
        constexpr auto& operator--() noexcept { --m_Value; return *this; }
        constexpr value_type operator++(int) noexcept { return m_Value++; }
        constexpr value_type operator--(int) noexcept { return m_Value--; }

        [[nodiscard]] constexpr value_type operator-() const noexcept { return -m_Value; }
        [[nodiscard]] constexpr value_type operator~() const noexcept { return ~m_Value; }


#define PX_CONVAR_OPEARTOR(Operator)														\
	    constexpr con_var_t& operator##Operator##=(const con_var_t& cvar) noexcept			\
	    {																					\
		    m_Value Operator##= cvar.m_Value;												\
		    return *this;																	\
	    }																					\
	    template<typename _OTy>																\
	    constexpr con_var_t& operator##Operator##=(const _OTy& o)	noexcept				\
	    {																					\
		    m_Value Operator##= static_cast<const_reference>(o);							\
		    return *this;																	\
	    }																					\
	    [[nodiscard]] constexpr auto operator##Operator(const con_var_t& cvar) const noexcept\
	    {																					\
		    return m_Value Operator cvar.m_Value;											\
	    }																					\
	    template<typename _OTy>																\
	    [[nodiscard]] constexpr auto operator##Operator(const _OTy& o) const noexcept		\
	    {																					\
		    return m_Value Operator static_cast<const_reference>(o);						\
	    }

        PX_CONVAR_OPEARTOR(+);
        PX_CONVAR_OPEARTOR(-);
        PX_CONVAR_OPEARTOR(/);
        PX_CONVAR_OPEARTOR(*);
        PX_CONVAR_OPEARTOR(%);
        PX_CONVAR_OPEARTOR(&);
        PX_CONVAR_OPEARTOR(| );
        PX_CONVAR_OPEARTOR(^);
        PX_CONVAR_OPEARTOR(<<);
        PX_CONVAR_OPEARTOR(>>);

#undef PX_CONVAR_OPEARTOR

    public:
        [[nodiscard]] auto data() const noexcept
        {
            if constexpr (requires(const_reference v) { v.data(); })
                return m_Value.data();
            else
                return &m_Value;
        }

        [[nodiscard]] auto data() noexcept
        {
            if constexpr (requires(reference v) { v.data(); })
                return m_Value.data();
            else
                return &m_Value;
        }

        [[nodiscard]] auto begin() const noexcept
        {
            if constexpr (requires(const_reference v) { std::begin(v); })
                return std::begin(m_Value);
            else
                return &m_Value;
        }

        [[nodiscard]] auto begin() noexcept
        {
            if constexpr (requires(reference v) { std::begin(v); })
                return std::begin(m_Value);
            else
                return &m_Value;
        }

        [[nodiscard]] auto end() const noexcept
        {
            if constexpr (requires(const_reference v) { std::end(v); })
                return std::end(m_Value);
            else
                return begin() + sizeof(value_type);
        }

        [[nodiscard]] string_t str() const
        {
            return cmd_arg_info<_Ty>::to_string(get(), traits_type::default_split_str);
        }

    public:
        [[nodiscard]] pointer operator->() noexcept { return &m_Value; }
        [[nodiscard]] constexpr const_pointer operator->() const noexcept { return &m_Value; }

        template<typename _IdxType = ptrdiff_t>
        [[nodiscard]] const auto& operator[](const _IdxType& _idx) const noexcept
        {
            return m_Value[_idx];
        }

        template<typename _IdxType = ptrdiff_t>
        [[nodiscard]] auto& operator[](const _IdxType& _idx) noexcept
        {
            return m_Value[_idx];
        }

        void operator()() = delete;
        void operator()(cmd_arg_t<traits_type>&& args) = delete;
        void operator()(cmd_valinfo_t<traits_type>&& value) = delete;
        void operator()(cmd_arg_t<traits_type>&& args, cmd_valinfo_t<traits_type>&& value) = delete;

        static void default_command_cvar_min(parent_type* command, const con_exec_info_t<traits_type>& info) noexcept
        {
            if (const auto min_iter = info.args.find("min"); min_iter != info.args.end())
            {
                con_var_t& cvar = static_cast<con_var_t&>(*command);
                if (auto val = min_iter->second.get_as<value_type>(); val > cvar)
                    cvar = std::move(val);
            }
        }

        static void default_command_cvar_max(parent_type* command, const con_exec_info_t<traits_type>& info) noexcept
        {
            if (const auto max_iter = info.args.find("max"); max_iter != info.args.end())
            {
                con_var_t& cvar = static_cast<con_var_t&>(*command);
                if (auto val = max_iter->second.get_as<value_type>(); val < cvar)
                    cvar = std::move(val);
            }
        }

        template<bool _Min = false, bool _Max = false>
        static void default_command_cvar_set(parent_type* command, const con_exec_info_t<traits_type>& info) noexcept
        {
            static_cast<con_var_t*>(command)->m_Value = info.value.get_as<value_type>();
            if constexpr (_Min)
                default_command_cvar_min(command, info);
            if constexpr (_Max)
                default_command_cvar_max(command, info);
        }

    private:
        value_type m_Value;
    };

    template<typename _CmdArgTrait_t>
    struct [[nodiscard]] con_exec_info_t
    {
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;

        cmd_arg_t<traits_type> args;
        cmd_valinfo_t<traits_type> value;

        con_exec_info_t() = default;
        con_exec_info_t(cmd_arg_t<traits_type>&& args) :
            args(std::move(args)) {}
        con_exec_info_t(cmd_valinfo_t<traits_type>&& value) :
            value(std::move(value)) {}
        con_exec_info_t(cmd_arg_t<traits_type>&& args, cmd_valinfo_t<traits_type>&& value) :
            args(std::move(args)), value(std::move(value)) {}
    };

    using con_command    = con_command_t<cmd_parser_default_traits>;
    using con_u8command  = con_command_t<cmd_parser_default_u8traits>;
    using con_wcommand   = con_command_t<cmd_parser_default_wtraits>;
    using con_u16command = con_command_t<cmd_parser_default_u16traits>;
    using con_u32command = con_command_t<cmd_parser_default_u32traits>;

    template<typename _Ty> using con_var    = con_var_t<_Ty, cmd_parser_default_traits>;
    template<typename _Ty> using con_u8var  = con_var_t<_Ty, cmd_parser_default_u8traits>;
    template<typename _Ty> using con_wvar   = con_var_t<_Ty, cmd_parser_default_wtraits>;
    template<typename _Ty> using con_u16var = con_var_t<_Ty, cmd_parser_default_u16traits>;
    template<typename _Ty> using con_u32var = con_var_t<_Ty, cmd_parser_default_u32traits>;
    
    using con_exec_info     = con_exec_info_t<cmd_parser_default_traits>;
    using con_exec_u8info   = con_exec_info_t<cmd_parser_default_u8traits>;
    using con_exec_winfo    = con_exec_info_t<cmd_parser_default_wtraits>;
    using con_exec_u16info  = con_exec_info_t<cmd_parser_default_u16traits>;
    using con_exec_u32info  = con_exec_info_t<cmd_parser_default_u32traits>;

}
