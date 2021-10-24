#pragma once

#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN();

class IAppSystem
{
public:
	virtual bool Connect(void* (*factory)(const char*, int*)) abstract;
	virtual void Disconnect() abstract;

	virtual void* QueryInterface(const char* pInterfaceName) abstract;

	virtual int Init() abstract;
	virtual void Shutdown() abstract;
};

TF2_NAMESPACE_END();
