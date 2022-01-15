#pragma once

#include <tf2/config.hpp>

TF2_NAMESPACE_BEGIN(::utils)

#ifdef _WIN32
#define PX_MAKE_DLL(NAME)	NAME##".dll"
#elif defined __linux__
#define PX_MAKE_DLL(NAME)	NAME##".so"
#else
#define PX_MAKE_DLL(...)
#endif

#define TF2_ENGINE_DLL		PX_MAKE_DLL("engine")
#define TF2_CLIENT_DLL		PX_MAKE_DLL("client")
#define TF2_SERVER_DLL		PX_MAKE_DLL("server")
#define TF2_VALVESTD_DLL	PX_MAKE_DLL("vstdlib")
#define TF2_MATSURFACE_DLL	PX_MAKE_DLL("vguimatsurface")


#include <type_traits>
#include <assert.h>

class _empty_callclass { };

template<typename _Ty>
constexpr _empty_callclass* to_empty_class(const _Ty* cls) noexcept
{
    return std::bit_cast<_empty_callclass*>(const_cast<_Ty*>(cls));
}

enum class callconv_t : char
{
    cc_cdecl,
    cc_stdcall,
    cc_fastcall,
#if defined _M_CEE_PURE || defined MRTDLL
    cc_clrcall,
#endif
    cc_vectorcall,
    cc_thiscall,

    cc_winapi = cc_stdcall
};

template<typename _RTy, callconv_t conv = callconv_t::cc_cdecl, typename... _Args>
class FuncThunkEx
{
public:
    using return_type = _RTy;
    using args_pack = std::tuple<_Args...>;
    using func_type = 
        std::conditional_t<conv == callconv_t::cc_cdecl, return_type(_cdecl*)(_Args...),
            std::conditional_t<conv == callconv_t::cc_stdcall, return_type(_stdcall*)(_Args...),
                std::conditional_t<conv == callconv_t::cc_fastcall, return_type(_fastcall*)(_Args...),
                    std::conditional_t<conv == callconv_t::cc_vectorcall, return_type(_vectorcall*)(_Args...),
                        std::conditional_t< conv == callconv_t::cc_thiscall, return_type(__thiscall*)(_Args...),
#if defined _M_CEE_PURE || defined MRTDLL
                        std::conditional_t<conv == callconv_t::cc_clrcall, return_type(__clrcall*)(_Args...),
                        void>
#else
                        void
#endif
                    >
                >
            >
        >
    >;

    constexpr FuncThunkEx(void* ptr = nullptr) noexcept : _U({ .ptr = ptr }) { }

    constexpr operator bool() const noexcept
    {
        return get() != nullptr;
    }

    constexpr return_type operator()(_Args... args)
    {
        assert(get());
        return (*_U.func)(std::forward<_Args>(args)...);
    }

    void set(void* ptr) noexcept
    {
        _U.ptr = ptr;
    }

    constexpr void* get() const noexcept
    {
        return _U.ptr;
    }
private:
    union {
        func_type func;
        void* ptr;
    } _U;
};

template<typename _RTy, typename... _Args>
class MemberFuncThunk
{
public:
    using return_type = _RTy;
    using args_pack = std::tuple<_Args...>;
    using func_type = return_type(_empty_callclass::*)(_Args...);

    constexpr MemberFuncThunk(void* ptr = nullptr) noexcept : _U({ .ptr = ptr }) { }

    constexpr operator bool() const noexcept
    {
        return get() != nullptr;
    }

    constexpr return_type operator()(const void* this_ptr, _Args... args)
    {
        assert(get());
        return (to_empty_class(this_ptr)->*_U.func)(
            std::forward<_Args>(args)...
        );
    }

    void set(void* ptr) noexcept
    {
        _U.ptr = ptr;
    }

    constexpr void* get() const noexcept
    {
        return _U.ptr;
    }

private:
    union {
        func_type func;
        void* ptr;
    } _U;
};

template<typename _RTy, typename... _Args>
class MemberVFuncThunk
{
public:
    using return_type = _RTy;
    using args_pack = std::tuple<_Args...>;
    using func_type = return_type(_empty_callclass::*)(_Args...);

    constexpr MemberVFuncThunk(int offset = -1) noexcept : m_Offset(offset) { };

    constexpr operator bool() const noexcept
    {
        return get() != -1;
    }

    constexpr return_type operator()(const void* this_ptr, _Args... args)
    {
        assert(get()  != -1);

        union {
            func_type func;
            void* ptr;
        } u{ .ptr = (*(void***)(this_ptr))[get()] };

        return (to_empty_class(this_ptr)->*u.func)(
            std::forward<_Args>(args)...
        );
    }

    void set(int offset) noexcept
    {
        m_Offset = offset;
    }

    int get() const noexcept
    {
        return m_Offset;
    }

private:
    int m_Offset{ };
};

template<typename _RTy, typename... _Args>
using FuncThunk = FuncThunkEx<_RTy, callconv_t::cc_cdecl, _Args...>;

TF2_NAMESPACE_END();