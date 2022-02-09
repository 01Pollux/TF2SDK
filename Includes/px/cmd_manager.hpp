#pragma once

#include "cmd_parser.hpp"

namespace px
{
    /// <summary>
    /// class that handles commands per module/program
    /// 
    /// each command/cvar (console variable) must be either used globally or one should manage their lifetime with linking it with px::cmd_guard_t
    /// 
    /// to declare a cvar:
    ///     px::con_var<int> my_int("somename");
    ///     px::con_var<std::array<int, 3>> my_array("some_array", { 1, 2, 3 });
    ///     px::con_var<std::string> my_string("string", "hah", "set string");
    /// 
    /// to use a cvar via console: (assuming we implemented the cmd_manager_t::execute command in main program's loop)
    ///     "somename 10"           -> my_int = 10
    ///     "some_array 10;-;2;-;4" -> my_array = { 1, 2, 3 }
    ///     "some_array"            -> my_string = ""
    /// 
    /// to declare a command:
    ///     either use same thing as cvar, except the second argument is execution callback
    ///     px::con_command do_stuff("do_stuff", [](auto, const auto&) { std::cout << "hello from do_stuff\n"; });
    ///     or use PX_COMMAND macro
    ///     PX_COMMAND(do_other)
    ///     {
    ///         // exec_info is a struct of 'args' and 'value',
    ///         // .get converts the value from string to the type
    ///         // and .str gets you the value directly
    ///         std::cout << exec_info.value.str();
    ///     }
    /// 
    /// to use a command via console: (assuming we implemented the cmd_manager_t::execute command in main program's loop)
    ///     "do_stuff --help -t 102 -a "hello world" -b "" "value1" "value2.0 value2.5" value3" 
    ///     "do_other --help -c "value1" "value2.0 value2.5" value3"
    /// 
    ///     note for flags: to only check for flags existence and if we dont care about its value, like b and c from above
    ///     we can either pass empty string like b
    ///     or apply 'ignore_value' mask for second command (check px::cmd_mask)
    /// 
    /// </summary>
    template<typename _CmdArgTrait_t>
    class cmd_manager_t
    {
    public:
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;

        using command_type  = con_command_t<traits_type>;
        template<typename _Ty>
        using cvar_type     = con_var_t<_Ty, traits_type>;
        using parser_type   = cmd_parser_t<traits_type>;
        using exec_type     = parser_type::exec_type;

        [[nodiscard]] static command_type* find(const string_t& str)
        {
            auto iter = command_type::m_Commands.find(str);
            return iter == command_type::m_Commands.end() ? nullptr : iter->second;
        }

        [[nodiscard]] static command_type* find(string_view_t str)
        {
            return find(string_t{ str });
        }

        [[nodiscard]] static command_type* find_command(const string_t& str)
        {
            auto cmd = find(str);
            return cmd && cmd->is_command() ? cmd : nullptr;
        }

        [[nodiscard]] static command_type* find_command(string_view_t str)
        {
            return find_command(string_t{ str });
        }

        template<typename _Ty>
        [[nodiscard]] static cvar_type<_Ty>* find_var(string_view_t str)
        {
            auto cmd = find(str);
            return cmd && !cmd->is_command() ? cmd : nullptr;
        }

        template<typename _Ty>
        [[nodiscard]] static cvar_type<_Ty>* find_var(const string_t& str)
        {
            return find_var<_Ty>(string_t{ str });
        }

        static void erase(string_view_t str)
        {
            command_type::m_Commands.erase(string_t{ str });
        }

        /// <summary>
        /// find command by name an call it exec_info for its callback
        /// </summary>
        /// <param name="exec_info"></param>
        static void execute(const std::vector<string_view_t>& commands, const std::vector<const exec_type>& exec_info)
        {
            if (commands.size() != exec_info.size())
                return;
            for (size_t i = 0; i < commands.size(); i++)
                execute(string_t{ commands[i] }, exec_info[i]);
        }

        /// <summary>
        /// execute a command by its name
        /// </summary>
        template<typename... _Args>
        static void execute(const string_t& command, _Args&&... info)
        {
            auto concmd_iter = command_type::m_Commands.find(command);
            if (concmd_iter != command_type::m_Commands.end())
                (*concmd_iter->second)(std::forward<_Args>(info)...);
        }

        [[nodiscard]] static auto begin() noexcept
        {
            return command_type::m_Commands.begin();
        }
        [[nodiscard]] static auto end() noexcept
        {
            return command_type::m_Commands.end();
        }
        [[nodiscard]] static auto rbegin() noexcept
        {
            return command_type::m_Commands.rbegin();
        }
        [[nodiscard]] static auto rend() noexcept
        {
            return command_type::m_Commands.rend();
        }

        static void clear()
        {
            command_type::m_Commands.clear();
        }
    };

    using cmd_manager   = cmd_manager_t<cmd_parser_default_traits>;
    using cmd_u8manager = cmd_manager_t<cmd_parser_default_u8traits>;
    using cmd_wmanager  = cmd_manager_t<cmd_parser_default_wtraits>;
    using cmd_u16manager= cmd_manager_t<cmd_parser_default_u16traits>;
    using cmd_u32manager= cmd_manager_t<cmd_parser_default_u32traits>;

    template<typename _CmdArgTrait_t>
    class [[nodiscard]] cmd_guard_t
    {
    public:
        using traits_type   = _CmdArgTrait_t;
        using string_t      = typename traits_type::string_t;
        using string_view_t = typename traits_type::string_view_t;
        using char_t        = typename traits_type::char_t;

        using command_type = con_command_t<traits_type>;
        using manager_type = cmd_manager_t<traits_type>;

        cmd_guard_t() = default;
        cmd_guard_t(command_type& cmd) noexcept : m_Command(&cmd) {};

        ~cmd_guard_t()
        {
            if (m_Command)
                manager_type::erase(m_Command->name());
        }
    private:
        command_type* m_Command{ };
    };
    
    using cmd_guard     = cmd_guard_t<cmd_parser_default_traits>;
    using cmd_u8guard   = cmd_guard_t<cmd_parser_default_u8traits>;
    using cmd_wguard    = cmd_guard_t<cmd_parser_default_wtraits>;
    using cmd_u16guard  = cmd_guard_t<cmd_parser_default_u16traits>;
    using cmd_u32guard  = cmd_guard_t<cmd_parser_default_u32traits>;
}


#define PX_COMMAND_T(Type, Name, ...)                                                \
void Name##_Callback(px::Type* command, const px::Type::exec_info_type& exec_info);  \
static px::Type Name##_cmd(                                                          \
    #Name,                                                                           \
    Name##_Callback,                                                                 \
    __VA_ARGS__                                                                      \
);                                                                                   \
void Name##_Callback(px::Type* command, const px::Type::exec_info_type& exec_info) 

#define PX_COMMAND(Name, ...)       PX_COMMAND_T(con_command, Name, __VA_ARGS__)

#define PX_U8COMMAND(Name, ...)     PX_COMMAND_T(con_u8command, Name, __VA_ARGS__)

#define PX_WCOMMAND(Name, ...)      PX_COMMAND_T(con_wcommand, Name, __VA_ARGS__)

#define PX_U16COMMAND(Name, ...)    PX_COMMAND_T(con_u16command, Name, __VA_ARGS__)

#define PX_UU32COMMAND(Name, ...)   PX_COMMAND_T(con_u32command, Name, __VA_ARGS__)
