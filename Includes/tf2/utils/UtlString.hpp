#pragma once

#include <memory>
#include <tf2/config.hpp>

TF2_NAMESPACE_BEGIN(::utils);

class UtlString
{
public:
	UtlString() = default;

	UtlString(const std::string_view& str)
	{
		set(str);
	}

	UtlString(const UtlString& o)
	{
		set(o.m_Data);
	}

	UtlString(UtlString&& o) noexcept
	{
		m_Data = std::exchange(o.m_Data, nullptr);
	}

	UtlString& operator=(const UtlString& o)
	{
		if (this != &o)
			set(o.m_Data);
		return *this;
	}

	UtlString& operator=(UtlString&& o) noexcept
	{
		if (this != &o)
		{
			if (m_Data)
				free(m_Data);
			m_Data = std::exchange(o.m_Data, nullptr);
		}
		return *this;
	}

	[[nodiscard]] size_t length() const
	{
		return m_Data ? strlen(m_Data) : 0;
	}

	[[nodiscard]] bool empty() const noexcept
	{
		return m_Data ? !m_Data[0] : true;
	}

	operator const char*() const noexcept
	{
		return m_Data;
	}

	operator const char*() noexcept
	{
		return m_Data;
	}

	[[nodiscard]] char operator[](int i) const noexcept
	{
		return m_Data[i];
	}

	[[nodiscard]] char& operator[](int i) noexcept
	{
		return m_Data[i];
	}

	[[nodiscard]] const char* c_str() const noexcept
	{
		return m_Data;
	}

	[[nodiscard]] std::string str() const noexcept
	{
		return m_Data;
	}

	[[nodiscard]] const char* data() const noexcept
	{
		return m_Data;
	}

	[[nodiscard]] char* data() noexcept
	{
		return m_Data;
	}

private:
	void set(const std::string_view& str)
	{
		if (m_Data)
		{
			if (strlen(m_Data) <= str.size())
				m_Data = std::bit_cast<char*>(realloc(m_Data, str.size() + 1));
		}
		else
		{
			m_Data = std::bit_cast<char*>(malloc(str.size() + 1));
		}
		memcpy_s(m_Data, str.size(), str.data(), str.size());
		m_Data[str.size()] = '\0';
	}

private:
	char* m_Data{ };
};

TF2_NAMESPACE_END();