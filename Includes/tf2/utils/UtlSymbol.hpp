#pragma once

#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN(::utils);


class UtlSymbol
{
public:
	// constructor, destructor
	UtlSymbol() : m_ID(std::numeric_limits<uint16_t>::max()) {}
	UtlSymbol(uint16_t id = std::numeric_limits<uint16_t>::max()) noexcept : m_ID(id) {}
	UtlSymbol(const UtlSymbol&) = default;

	// operator=
	UtlSymbol& operator=(const UtlSymbol& src) { m_ID = src.m_ID; return *this; }

	// operator==
	bool operator<=>(const UtlSymbol&) const = default;

	// Is valid?
	bool IsValid() const { return m_ID != std::numeric_limits<uint16_t>::max(); }

	// Gets at the symbol
	operator uint16_t() const noexcept { return m_ID; }

protected:
	uint16_t  m_ID;
};


TF2_NAMESPACE_END();
