
#include <tf2/engine/Convar.hpp>

TF2_NAMESPACE_BEGIN();


namespace ConCommandHelper
{
	static VCVarDLLIdentifier DLLIdentifier = -1;
	static bool ConBaseRegistered = false;
	static uint32_t ConBaseFlags = 0;


	class CCDefaultAccessor : public IConCommandBaseAccessor
	{
	public:
		bool RegisterConCommandBase(ConCommandBase* pVar) final
		{
			Interfaces::CVar->RegisterConCommand(pVar);
			return true;
		}
	};
	static CCDefaultAccessor DefaultAccessor;
}

void ConCommandBase::Register(uint32_t nCVarFlag, IConCommandBaseAccessor* pAccessor)
{
	assert(!(Interfaces::CVar || ConCommandHelper::ConBaseRegistered));
	if (!Interfaces::CVar || ConCommandHelper::ConBaseRegistered)
		return;

	ConCommandHelper::ConBaseRegistered = true;
	ConCommandHelper::ConBaseFlags = nCVarFlag;
	ConCommandHelper::DLLIdentifier = Interfaces::CVar->AllocateDLLIdentifier();

	ConCommandBase::Accessor = pAccessor ? pAccessor : &ConCommandHelper::DefaultAccessor;
	ConCommandBase* pCur = ConCommandBase::ConCommandBases;

	while (pCur)
	{
		ConCommandBase* pNext = pCur->NextBase;

		pCur->AddFlags(nCVarFlag);
		pCur->Init();

		pCur = pNext;
	}

	Interfaces::CVar->ProcessQueuedMaterialThreadConVarSets();
	ConCommandBase::ConCommandBases = nullptr;
}

void ConCommandBase::Unregister()
{
	assert(!(Interfaces::CVar || ConCommandHelper::ConBaseRegistered));
	if (!Interfaces::CVar || !ConCommandHelper::ConBaseRegistered)
		return;

	Interfaces::CVar->UnregisterConCommands(ConCommandHelper::DLLIdentifier);
	ConCommandHelper::DLLIdentifier = -1;
	ConCommandHelper::ConBaseRegistered = false;
}


VCVarDLLIdentifier ConCommandBase::GetDLLIdentifier() const
{
	return ConCommandHelper::DLLIdentifier;
}


void ConCommandBase::CreateBase(const char* pName, const char* pHelpString, uint32_t flags)
{
	Is_Registered = false;

	Name = pName;
	HelpString = pHelpString ? pHelpString : "";

	Flags = flags;

	if (!(Flags & Const::ConvarFlags::Unregistered))
	{
		NextBase = ConCommandBase::ConCommandBases;
		ConCommandBase::ConCommandBases = this;
	}
	else
		NextBase = nullptr;

	if (ConCommandBase::Accessor)
		Init();
}

void ConCommandBase::Shutdown()
{
	if (Interfaces::CVar)
		Interfaces::CVar->UnregisterConCommand(this);
}

inline std::string ConCommandBase::CopyString(const char* from)
{
	return { from };
}


ConCommand::ConCommand(const char* pName, ConCommandCallbackV callback, const char* pHelpString, uint32_t flags, ConCommandCompletionCallback completionFunc) :
	VoidCallback(callback),
	UsingNewCommandCallback(false),
	UsingCommandCallbackInterface(false),
	CompletionCallback(completionFunc ? completionFunc : +[](const char*, char[64][64])->int { return 0; }),
	HasCompletionCallback(completionFunc ? true : false)
{
	ConCommandBase::CreateBase(pName, pHelpString, flags);
}

ConCommand::ConCommand(const char* pName, ConCommandCallback callback, const char* pHelpString, uint32_t flags, ConCommandCompletionCallback completionFunc) :
	Callback(callback),
	UsingNewCommandCallback(true),
	UsingCommandCallbackInterface(false),
	CompletionCallback(completionFunc ? completionFunc : +[](const char*, char[64][64])->int { return 0; }),
	HasCompletionCallback(completionFunc ? true : false)
{
	ConCommandBase::CreateBase(pName, pHelpString, flags);
}

int ConCommand::AutoCompleteSuggest(const char* partial, Utils::UtlVector<const char*>& commands)
{
	if (UsingCommandCallbackInterface)
		return 0;

	if (!CompletionCallback)
		return 0;

	char cmds[64][64];

	int iret = CompletionCallback(partial, cmds);

	for (int i = 0; i < iret; ++i)
		commands.push_to_tail(cmds[i]);

	return iret;
}

void ConCommand::Dispatch(const CCommand& command)
{
	if (UsingNewCommandCallback)
	{
		if (Callback)
			Callback(command);
	}
	else if (!UsingCommandCallbackInterface)
	{
		if (VoidCallback)
			VoidCallback();
	}
}



void ConVar::InternalSetValue(const char* value)
{
	if (IsFlagSet(Const::ConvarFlags::MatThreadMask))
	{
		if (Interfaces::CVar && !Interfaces::CVar->IsMaterialThreadSetAllowed())
		{
			Interfaces::CVar->QueueMaterialThreadSetValue(this, value);
			return;
		}
	}

	float fNewValue;
	char  tempVal[32];

	float flOldValue = FloatValue;

	char* val = const_cast<char*>(value);
	if (!value)
		fNewValue = 0.0f;
	else
		fNewValue = static_cast<float>(atof(value));

	if (ClampValue(fNewValue))
	{
		snprintf(tempVal, sizeof(tempVal), "%f", fNewValue);
		val = tempVal;
	}

	FloatValue = fNewValue;
	IntValue = static_cast<int>(fNewValue);

	if (!IsFlagSet(Const::ConvarFlags::NotString))
		ChangeStringValue(val, flOldValue);
}

void ConVar::InternalSetFloatValue(float fNewValue, bool force)
{
	if (fNewValue == FloatValue && force)
		return;

	if (IsFlagSet(Const::ConvarFlags::MatThreadMask))
	{
		if (Interfaces::CVar && !Interfaces::CVar->IsMaterialThreadSetAllowed())
		{
			Interfaces::CVar->QueueMaterialThreadSetValue(this, fNewValue);
			return;
		}
	}

	ClampValue(fNewValue);

	float flOldValue = FloatValue;
	FloatValue = fNewValue;
	IntValue = static_cast<int>(fNewValue);

	if (!IsFlagSet(Const::ConvarFlags::NotString))
	{
		char tempVal[32];
		snprintf(tempVal, sizeof(tempVal), "%f", FloatValue);
		ChangeStringValue(tempVal, flOldValue);
	}
}

void ConVar::InternalSetIntValue(int nValue)
{
	if (nValue == IntValue)
		return;

	if (IsFlagSet(Const::ConvarFlags::MatThreadMask))
	{
		if (Interfaces::CVar && !Interfaces::CVar->IsMaterialThreadSetAllowed())
		{
			Interfaces::CVar->QueueMaterialThreadSetValue(this, nValue);
			return;
		}
	}

	float fValue = static_cast<float>(nValue);
	if (ClampValue(fValue))
		nValue = static_cast<int>(fValue);

	float flOldValue = FloatValue;
	FloatValue = fValue;
	IntValue = nValue;

	if (!IsFlagSet(Const::ConvarFlags::NotString))
	{
		char tempVal[32];
		snprintf(tempVal, sizeof(tempVal), "%d", IntValue);
		ChangeStringValue(tempVal, flOldValue);
	}
}

void ConVar::Create(
	const char* pName, const char* pDefaultValue, uint32_t flags,
	const char* pHelpString, bool bMin, float fMin,
	bool bMax, float fMax, bool bCompMin,
	float fCompMin, bool bCompMax, float fCompMax,
	ConVarChangeCallback callback)
{
	Parent = this;

	SetDefault(pDefaultValue);

	StringValueLen = strlen(pDefaultValue) + 1;
	StringValue = std::make_unique<char[]>(StringValueLen);
	memcpy(StringValue.get(), DefaultValue, StringValueLen);

	HasMin = bMin;
	MinValue = fMin;
	HasMax = bMax;
	MaxValue = fMax;

	HasCompMin = bCompMin;
	CompMinValue = fCompMin;
	HasCompMax = bCompMax;
	CompMaxValue = fCompMax;

	IsCompetitiveRestrictions = false;

	ChangeCallback = callback;

	FloatValue = static_cast<float>(atof(StringValue.get()));
	IntValue = static_cast<int>(atoi(StringValue.get()));

	ConCommandBase::CreateBase(pName, pHelpString, flags);
}

bool ConVar::ClampValue(float& value)
{
	if (IsCompetitiveRestrictions)
	{
		if (HasCompMin && (value < CompMinValue))
		{
			value = CompMinValue;
			return true;
		}

		if (HasCompMax && (value > CompMaxValue))
		{
			value = CompMaxValue;
			return true;
		}

		if (!HasCompMin && !HasCompMax)
		{
			float fDefaultAsFloat = static_cast<float>(atof(DefaultValue));
			if (fabs(value - fDefaultAsFloat) > .0001f)
			{
				value = fDefaultAsFloat;
				return true;
			}
		}
	}

	if (HasMin && (value < MinValue))
	{
		value = MinValue;
		return true;
	}

	if (HasMax && (value > MaxValue))
	{
		value = MaxValue;
		return true;
	}

	return false;
}

void ConVar::ChangeStringValue(const char* tempVal, float flOldValue)
{
	if (tempVal)
	{
		size_t len = strlen(tempVal) + 1;

		if (len > StringValueLen)
		{
			StringValue.reset(new char[len]);
			StringValueLen = len;
		}

		memcpy(StringValue.get(), tempVal, len);
	}
	else
	{
		StringValue[0] = 0;
	}

	auto oldVal(std::make_unique<char[]>(StringValueLen));
	memcpy(oldVal.get(), StringValue.get(), StringValueLen);

	// If nothing has changed, don't do the callbacks.
	if (strcmp(oldVal.get(), StringValue.get()))
	{
		// Invoke any necessary callback function
		if (ChangeCallback)
			ChangeCallback(this, oldVal.get(), flOldValue);

		Interfaces::CVar->CallGlobalChangeCallbacks(this, oldVal.get(), flOldValue);
	}
}

TF2_NAMESPACE_END();
