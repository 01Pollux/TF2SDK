#pragma once

#include <memory>

#include "CVar.hpp"
#include <tf2/utils/UtlVector.hpp>

TF2_NAMESPACE_BEGIN();

namespace interfaces
{
	TF2_EXPORT_INTERFACE(ICVar, CVar, "VEngineCvar004");
}

class CCommand;

using ConCommandCallbackV = void(*)();
using ConCommandCallback = void(*)(const CCommand&);
using ConCommandCompletionCallback = int(*)(const char* partial, char commands[64][64]);



class ConCommand : public ConCommandBase
{
	friend class ICVar;

public:
	PX_SDK_TF2 ConCommand(const char* pName, ConCommandCallbackV callback,
		const char* pHelpString = nullptr, uint32_t flags = Const::ConvarFlags::None, ConCommandCompletionCallback completionFunc = nullptr);
	PX_SDK_TF2 ConCommand(const char* pName, ConCommandCallback callback,
		const char* pHelpString = nullptr, uint32_t flags = Const::ConvarFlags::None, ConCommandCompletionCallback completionFunc = nullptr);

	virtual ~ConCommand() = default;

	bool IsCommand() const noexcept override { return true; }

	PX_SDK_TF2 virtual int 
		AutoCompleteSuggest(const char* partial, utils::UtlVector<const char*>& commands);

	virtual bool CanAutoComplete() { return HasCompletionCallback; }

	PX_SDK_TF2 virtual void 
		Dispatch(const CCommand& command);

	ConCommand(const ConCommand&) = default;
	ConCommand& operator=(const ConCommand&) = default;
	ConCommand(ConCommand&&) = default;
	ConCommand& operator=(ConCommand&&) = default;
private:
	union
	{
		ConCommandCallbackV VoidCallback;
		ConCommandCallback Callback;
	};

	ConCommandCompletionCallback CompletionCallback;

	bool HasCompletionCallback : 1;
	bool UsingNewCommandCallback : 1;
	bool UsingCommandCallbackInterface : 1;
};


class ConVar : public ConCommandBase, public ConVar_Internal
{
public:
	ConVar(
		const char* pName, const char* pDefaultValue, uint32_t flags = Const::ConvarFlags::None,
		const char* pHelpString = nullptr)
	{
		Create(pName, pDefaultValue, flags, pHelpString);
	}

	ConVar(
		const char* pName, const char* pDefaultValue, uint32_t flags,
		const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax)
	{
		Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax);
	}

	ConVar(
		const char* pName, const char* pDefaultValue, uint32_t flags,
		const char* pHelpString, ConVarChangeCallback callback)
	{
		Create(pName, pDefaultValue, flags, pHelpString, false, .0f, false, .0f, false, .0f, false, .0f, callback);
	}

	ConVar(
		const char* pName, const char* pDefaultValue, uint32_t flags,
		const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax,
		ConVarChangeCallback callback)
	{
		Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, false, .0f, false, .0f, callback);
	}

	ConVar(
		const char* pName, const char* pDefaultValue, uint32_t flags,
		const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax,
		bool bCompMin, float fCompMin, bool bCompMax, float fCompMax,
		ConVarChangeCallback callback)
	{
		Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, bCompMin, fCompMin, bCompMax, fCompMax, callback);
	}


	virtual ~ConVar() = default;

	bool IsFlagSet(uint32_t flags)			const noexcept final { return (Parent->Flags & flags) == flags; }
	const char* GetHelpText()						const noexcept final { return Parent->HelpString; }
	bool IsRegistered()								const noexcept final { return Parent->Is_Registered; }
	const char* GetName()							const noexcept final { return Parent->Name; }
	void AddFlags(uint32_t flags)				  noexcept final { Parent->Flags |= flags; }
	bool IsCommand()								const noexcept final { return false; }

	void InstallChangeCallback(ConVarChangeCallback callback)
	{
		Parent->ChangeCallback = callback;
		if (callback)
			Parent->ChangeCallback(this, StringValue.get(), FloatValue);
	}

	// Retrieve value
	float GetFloat() const noexcept
	{
		return Parent->FloatValue;
	}
	int	GetInt() const noexcept
	{
		return Parent->IntValue;
	}
	bool GetBool() const noexcept
	{
		return GetInt() != 0;
	}
	const char* GetString() const noexcept
	{
		return Parent->StringValue.get();
	}

	// These just call into the IConCommandBaseAccessor to check flags and set the var (which ends up calling InternalSetValue).
	void SetValue(const char* value) override
	{
		Parent->InternalSetValue(value);
	}
	void SetValue(float value) override
	{
		Parent->InternalSetFloatValue(value);
	}
	void SetValue(int value) override
	{
		Parent->InternalSetIntValue(value);
	}

	void Revert()
	{
		Parent->SetValue(DefaultValue);
	}

	// True if it has a min/max setting
	bool GetMin(float& minVal) const noexcept
	{
		minVal = Parent->MinValue;
		return Parent->HasMin;
	}
	bool GetMax(float& maxVal) const noexcept
	{
		maxVal = Parent->MaxValue;
		return Parent->HasMax;
	}
	const char* GetDefault() const noexcept
	{
		return DefaultValue;
	}

	void SetDefault(const char* val) noexcept
	{
		DefaultValue = val ? val : "";
	}

	// True if it has a min/max competitive setting
	bool GetCompMin(float& minVal) const noexcept
	{
		minVal = Parent->CompMinValue;
		return Parent->HasCompMin;
	}
	bool GetCompMax(float& maxVal) const noexcept
	{
		maxVal = Parent->CompMaxValue;
		return Parent->HasCompMax;
	}

	ConVar(const ConVar&) = delete;
	ConVar& operator=(const ConVar&) = delete;
	ConVar(ConVar&&) = delete;
	ConVar& operator=(ConVar&&) = delete;

private:
	// Called by CCvar when the value of a var is changing.
	PX_SDK_TF2 virtual void 
		InternalSetValue(const char* value);
	// For CVARs marked FCVAR_NEVER_AS_STRING
	PX_SDK_TF2 virtual void
		InternalSetFloatValue(float fNewValue, bool bForce = false);
	PX_SDK_TF2 virtual void
		InternalSetIntValue(int nValue);

	PX_SDK_TF2 virtual bool
		ClampValue(float& value);
	PX_SDK_TF2 virtual void 
		ChangeStringValue(const char* tempVal, float flOldValue);

	PX_SDK_TF2 void 
		Create(
		const char* pName, const char* pDefaultValue, uint32_t flags = Const::ConvarFlags::None,
		const char* pHelpString = nullptr, bool bMin = false, float fMin = 0.f,
		bool bMax = false, float fMax = .0f,
		bool bCompMin = false, float fCompMin = .0f,
		bool bCompMax = false, float fCompMax = .0f,
		ConVarChangeCallback callback = 0
	);


	// Used internally by OneTimeInit to initialize.
	void Init() override
	{
		ConCommandBase::Init();
	}
	uint32_t GetFlags() { return Parent->Flags; }
public:

	// This either points to "this" or it points to the original declaration of a ConVar.
	// This allows ConVars to exist in separate modules, and they all use the first one to be declared.
	// m_pParent->m_pParent must equal m_pParent (ie: m_pParent must be the root, or original, ConVar).
	ConVar* Parent;

	const char* DefaultValue;

	std::unique_ptr<char[]>		StringValue;
	size_t						StringValueLen;

	float						FloatValue;
	int							IntValue;

	bool						HasMin;
	float						MinValue;
	bool						HasMax;
	float						MaxValue;

	bool						HasCompMin;
	float						CompMinValue;
	bool						HasCompMax;
	float						CompMaxValue;

	bool						IsCompetitiveRestrictions;


	ConVarChangeCallback		ChangeCallback;
};


struct CCmdCharacterSet
{
	char set[256];
};

class CCommand
{
public:
	int count() const noexcept
	{
		return ArgC;
	}

	bool has_count(int num) const noexcept
	{
		return num + 1 == ArgC;
	}

	bool has_atleast(int num) const noexcept
	{
		return num + 1 <= ArgC;
	}

	const char** argv() const noexcept
	{
		return ArgC ? (const char**)pArgV : nullptr;
	}

	// All args that occur after the 0th arg, in string form
	const char* str() const noexcept
	{
		return ArgV0Size ? &ArgSBuffer[ArgV0Size] : "";
	}

	// The entire command in string form, including the 0th arg
	const char* all() const noexcept
	{
		return ArgC ? ArgSBuffer : "";
	}

	// Gets at arguments
	const char* operator[](int index) const noexcept
	{
		return pArgV[index];
	}

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		ArgC;
	int		ArgV0Size;
	char	ArgSBuffer[COMMAND_MAX_LENGTH];
	char	ArgVBuffer[COMMAND_MAX_LENGTH];

	const char* pArgV[COMMAND_MAX_ARGC];
};


#define CMD_TAG "m01_"

#define M01_CONCOMMAND(NAME, FUNCTION_CALLBACK, ...) \
	static ConCommand NAME##_ccmd(CMD_TAG#NAME, FUNCTION_CALLBACK, __VA_ARGS__)



TF2_NAMESPACE_END();
