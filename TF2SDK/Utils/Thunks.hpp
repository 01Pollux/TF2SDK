#pragma once

#include "Config.hpp"

TF2_NAMESPACE_BEGIN(::Utils)

#ifdef _WIN32
#define SG_MAKE_DLL(NAME)	NAME##".dll"
#elif defined __linux__
#define SG_MAKE_DLL(NAME)	NAME##".so"
#else
#define SG_MAKE_DLL(...)
#endif

#define TF2_ENGINE_DLL		SG_MAKE_DLL("engine")
#define TF2_CLIENT_DLL		SG_MAKE_DLL("client")
#define TF2_SERVER_DLL		SG_MAKE_DLL("server")
#define TF2_VALVESTD_DLL	SG_MAKE_DLL("vstdlib")
#define TF2_MATSURFACE_DLL	SG_MAKE_DLL("vguimatsurface")


class CallClass { };

template<typename ReturnType, typename... Args>
class IFuncThunk
{
	using FnPtr = ReturnType(*)(Args...);
public:
	IFuncThunk(void* ptr = nullptr) noexcept { set(ptr); }

	constexpr operator bool() const noexcept
	{
		return Func != nullptr;
	}

	constexpr ReturnType operator()(Args... args)
	{
		return Func(args...);
	}

	void set(void* ptr) noexcept
	{
		union
		{
			FnPtr fn;
			void* ptr;
		} u{ .ptr = ptr };

		Func = u.fn;
	}

	void* get() const noexcept
	{
		union
		{
			FnPtr fn;
			void* ptr;
		} u{ .fn = Func };
		return u.ptr;
	}

private:
	FnPtr Func{ };
};

template<typename ReturnType, typename... Args>
class IMemberFuncThunk
{
	using FnPtr = ReturnType(CallClass::*)(Args...);
public:
	IMemberFuncThunk(void* ptr = nullptr) noexcept { set(ptr); }

	constexpr operator bool() const noexcept
	{
		return Func != nullptr;
	}

	constexpr ReturnType operator()(const void* thisptr, Args... args)
	{
		return ((CallClass*)thisptr->*Func)(args...);
	}

	void set(void* ptr) noexcept
	{
		union
		{
			FnPtr fn;
			void* ptr;
		} u{ .ptr = ptr };

		Func = u.fn;
	}

	void* get() const noexcept
	{
		union
		{
			FnPtr fn;
			void* ptr;
		} u{ .fn = Func };
		return u.ptr;
	}

private:
	FnPtr Func{ };
};


template<typename _RTy, typename... _Args>
class IMemberVFuncThunk
{
public:
	IMemberVFuncThunk(int offset = -1) noexcept : Offset(offset) { };

	constexpr operator bool() const noexcept
	{
		return Offset != -1;
	}

	constexpr _RTy operator()(const void* thisptr, _Args... args)
	{
		void** vtable = *(void***)(thisptr);
		IMemberFuncThunk<_RTy, _Args...> fn(vtable[Offset]);
		return fn(thisptr, args...);
	}

	void set(int offset) noexcept
	{
		Offset = offset;
	}

	int get() const noexcept
	{
		return Offset;
	}

private:
	int Offset{ };
};

TF2_NAMESPACE_END();
