#pragma once

#include "version.hpp"
#include <format>
#include <nlohmann/json.hpp>

namespace std
{
	template<class _CharT>
	struct formatter<px::version, _CharT>
	{
		template<typename _FmtCtx>
		auto parse(_FmtCtx& ctx)
		{
			return ctx.end();
		}

		template<typename _FmtCtx>
		auto format(const px::version& ver, _FmtCtx& fc)
		{
			return std::format_to(
				fc.out(),
				"{0}.{1}.{2}.{3}", 
				ver.major(), 
				ver.minor(),
				ver.build(),
				ver.revision()
			);
		}
	};

	template <class _CharT, class _Traits>
	basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& os, const px::version& ver)
	{
		return os << ver.major() << '.' << ver.minor() << '.' << ver.build() << '.' << ver.revision();
	}
}

namespace nlohmann
{
	inline void to_json(json& js, const px::version& version)
	{
		js = std::format("{}", version);
	}

	inline void from_json(const json& js, px::version& version)
	{
		version = px::version::from_string(js);
	}
}