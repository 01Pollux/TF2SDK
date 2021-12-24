#pragma once

#include <tf2/config.hpp>

TF2_NAMESPACE_BEGIN();

class IAppSystem
{
public:
	using CreateIFace = void* (*)(const char*, int*);

	enum class InitRetVal
	{
		Failed,
		Ok
	}; 
	
	virtual bool Connect(CreateIFace factory) abstract;
	virtual void Disconnect() abstract;

	virtual void* QueryInterface(const char* pInterfaceName) abstract;

	virtual InitRetVal Init() abstract;
	virtual void Shutdown() abstract;
};

class IBaseInterface
{
public:
	virtual	~IBaseInterface() = default;
};

TF2_NAMESPACE_END();
