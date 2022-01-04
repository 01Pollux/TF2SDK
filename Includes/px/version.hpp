#pragma once

#include <compare>
#include <array>
#include <string_view>

namespace px
{
	struct version
	{
	public:
		enum class type : unsigned char
		{
			Major,
			Minor,
			Build,
			Revision
		};
		using value_type = unsigned short;

		constexpr version() = default;

		constexpr version(value_type maj, value_type min, value_type build = 0, value_type rev = 0) noexcept :
			m_Major(maj), m_Minor(min), m_Build(build), m_Revision(rev) {  }

		constexpr version(const std::array<value_type, 4>& arr) noexcept :
			m_Major(arr[0]), m_Minor(arr[1]), m_Build(arr[2]), m_Revision(arr[3]) { }

		constexpr version(const std::string_view& str) noexcept :
			version(from_string(str)) {  }

		constexpr value_type operator[](type ty) const noexcept
		{
			switch (ty)
			{
			case type::Major:	return m_Major;
			case type::Minor:	return m_Minor;
			case type::Build:	return m_Build;
			case type::Revision:
			default:			return m_Revision;
			}
		}

		constexpr value_type& operator[](type ty) noexcept
		{
			switch (ty)
			{
			case type::Major:	return m_Major;
			case type::Minor:	return m_Minor;
			case type::Build:	return m_Build;
			case type::Revision:
			default:			return m_Revision;
			}
		}

		value_type& major() noexcept					{ return m_Major; }
		constexpr value_type major() const noexcept	{ return m_Major; }

		value_type& minor() noexcept					{ return m_Minor; }
		constexpr value_type minor() const noexcept	{ return m_Minor; }

		value_type& build() noexcept					{ return m_Build; }
		constexpr value_type build() const noexcept	{ return m_Build; }

		value_type& revision() noexcept				{ return m_Revision; }
		constexpr value_type revision() const noexcept{ return m_Revision; }

		constexpr auto to_array() const noexcept { return std::array{ major(), minor(), build(), revision() }; }

		static constexpr version from_string(const std::string_view& str)
		{
			int iter = 0;
			type cur_ver = type::Major;
			version ver;
			
			for (const char c : str)
			{
				if (cur_ver > type::Revision)
					break;

				if (c == '.')
				{
					iter = 0;
					cur_ver = static_cast<type>(static_cast<int>(cur_ver) + 1);
					continue;
				}

				iter *= 10;
				iter += c - '0';

				ver[cur_ver] = iter;
			}

			return ver;
		}

		constexpr auto operator<=>(const version&) const noexcept = default;

		static constexpr version Null() { return { }; }

	private:
		value_type
			m_Major = 0,
			m_Minor = 0,
			m_Build = 0,
			m_Revision = 0;
	};
}