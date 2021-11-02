#pragma once

#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN();

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() = default;

	virtual bool	IsReliable() const abstract;
	virtual bool	IsInitMessage() const abstract;

	virtual int		GetRecipientCount(void) const abstract;
	virtual int		GetRecipientIndex(int slot) const abstract;
};

TF2_NAMESPACE_END();
