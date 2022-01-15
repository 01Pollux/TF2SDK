#pragma once

#include <bit>
#include <numeric>
#include <compare>

#include <px/defines.hpp>

PX_NAMESPACE_BEGIN();

struct IntPtr
{
public:
	using type = uint8_t*;

	constexpr IntPtr(type iptr) noexcept : m_Ptr(iptr) { }
	constexpr IntPtr(void* ptr) noexcept : m_Ptr(std::bit_cast<type>(ptr)) { }
	constexpr IntPtr(std::nullptr_t) noexcept { }
	constexpr IntPtr() = default;

	[[nodiscard]] constexpr operator type() const noexcept { return m_Ptr; }
	//constexpr operator bool() const noexcept { return m_Ptr != 0; }

	template<typename _Ty>
	[[nodiscard]] _Ty as() const noexcept { return std::bit_cast<_Ty>(m_Ptr); }

	template<typename _Ty = void>
	[[nodiscard]] _Ty* get() const noexcept { return std::bit_cast<_Ty*>(m_Ptr); }

	template<typename _Ty>
	[[nodiscard]] _Ty read(int offset = 0) const noexcept { return *std::bit_cast<_Ty*>(m_Ptr + offset); }

	template<typename _Ty>
	[[nodiscard]] _Ty& read(int offset = 0) noexcept { return *std::bit_cast<_Ty*>(m_Ptr + offset); }

	template<typename _Ty>
	void write(const _Ty& d, int offset = 0) noexcept { *std::bit_cast<_Ty*>(m_Ptr + offset) = d; }

	IntPtr& operator+=(int b) noexcept { m_Ptr += b; return *this; };
	IntPtr& operator-=(int b) noexcept { m_Ptr -= b; return *this; };

	template<typename _Ty>
	IntPtr operator+(_Ty b) noexcept { IntPtr tmp = *this; tmp += b; return tmp; };
	template<typename _Ty>
	IntPtr operator-(_Ty b) noexcept { IntPtr tmp = *this; tmp -= b; return tmp; };

	IntPtr& operator++() noexcept { ++m_Ptr; return *this; };
	IntPtr operator++(int) noexcept { IntPtr tmp = *this; m_Ptr++; return tmp; };

	IntPtr operator--(int) noexcept { IntPtr tmp = *this; m_Ptr--; return tmp; };
	IntPtr& operator--() noexcept { --m_Ptr; return *this; };

	[[nodiscard]] constexpr auto operator<=>(const IntPtr&) const noexcept = default;

	[[nodiscard]] static constexpr IntPtr Null() { return { }; };

private:
	type m_Ptr{ };
};

PX_NAMESPACE_END();