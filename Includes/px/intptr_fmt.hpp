#pragma once

#include <format>
#include <px/intptr.hpp>

namespace std
{
	template<class _CharT>
	struct formatter<px::IntPtr, _CharT>
	{
		template<typename _FmtCtx>
		auto parse(_FmtCtx& ctx)
		{
			return ctx.end();
		}

		template<typename _FmtCtx>
		auto format(px::IntPtr p, _FmtCtx& fc)
		{
			return std::format_to(fc.out(), "{}", p.get());
		}
	};

	template <class _CharT, class _Traits>
	basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& os, const px::IntPtr& ptr)
	{
		return os << ptr.get();
	}
}