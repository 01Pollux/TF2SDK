#pragma once

#include <tf2/config.hpp>
#include "ConVar_Internal.hpp"

TF2_NAMESPACE_BEGIN();

class ConVar_Internal;
class ConVar;
class ConCommandBase;

using ConVarChangeCallback = void(*)(ConVar* var, const char* pOldValue, float flOldValue);
using VCVarDLLIdentifier = int;

namespace Const
{
	namespace ConvarFlags
	{
		static constexpr uint32_t None			= 0;

		// ConVar Systems
		// If this is set, don't add to linked list, etc.
		static constexpr uint32_t Unregistered	= 1 << 0;
		// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
		static constexpr uint32_t DevOnly		= 1 << 1;
		// defined by the game DLL
		static constexpr uint32_t GameDll		= 1 << 2;
		// defined by the client DLL
		static constexpr uint32_t ClientDll		= 1 << 3;
		// Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.
		static constexpr uint32_t Hidden		= 1 << 4;

		// It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
		static constexpr uint32_t Protected		= 1 << 5;
		// This cvar cannot be changed by clients connected to a multiplayer server.
		static constexpr uint32_t SPOnly		= 1 << 6;
		// set to cause it to be saved to vars.rc
		static constexpr uint32_t Archive		= 1 << 7;
		// notifies players when changed
		static constexpr uint32_t Notify		= 1 << 8;
		// changes the client's info string
		static constexpr uint32_t UserInfo		= 1 << 9;
		// Only useable in singleplayer / debug / multiplayer & sv_cheats
		static constexpr uint32_t Cheat			= 1 << 14;

		// This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
		static constexpr uint32_t PrintableOnly = 1 << 10;
		// If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
		static constexpr uint32_t Unlogged 		= 1 << 11;
		// never try to print that cvar
		static constexpr uint32_t NotString 	= 1 << 12;

		// It's a ConVar that's shared between the client and the server.
		// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
		//  client, of course )
		// If a change is requested it must come from the console (i.e., no remote client changes)
		// If a value is changed while a server is active, it's replicated to all connected clients
		// server setting enforced on clients, TODO rename to FCAR_SERVER at some time
		static constexpr uint32_t Replicated 	= 1 << 13;
		// record this cvar when starting a demo file
		static constexpr uint32_t Demo 			= 1 << 16;
		// don't record these command in demofiles
		static constexpr uint32_t NoRecord 		= 1 << 17;
		// If this cvar changes, it forces a material reload
		static constexpr uint32_t ReloadMaterials= 1 << 20;
		// If this cvar changes, if forces a texture reload
		static constexpr uint32_t ReloadTextures = 1 << 21;
		// cvar cannot be changed by a client that is connected to a server
		static constexpr uint32_t NotConntected = 1 << 22;
		// Indicates this cvar is read from the material system thread
		static constexpr uint32_t MatSysThread 	= 1 << 23;
		// cvar written to config.cfg on the Xbox
		static constexpr uint32_t ArchiveXBox 	= 1 << 24;
		// used as a debugging tool necessary to check material system thread convars
		static constexpr uint32_t AccessibleFromThreads 	= 1 << 25;
		// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
		static constexpr uint32_t ServerCanExecute 			= 1 << 28;
		// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
		static constexpr uint32_t ServerNoQuery 			= 1 << 29;
		// IVEngineClient::ClientCmd is allowed to execute this command. 
		// Note: IVEngineClient::ClientCmd_Unrestricted can run any client command.
		static constexpr uint32_t ClientCmdCanExecute 	= 1 << 30;
		// -default causes a lot of commands to be ignored (but still be recorded as though they had run). 
		// This causes them to be executed anyways.
		static constexpr uint32_t ExecDespiteDefault 	= 1 <<31;
		// This var isn't archived, but is exposed to players--and its use is allowed in competitive play.
		static constexpr uint32_t InternalUser 			= 1 << 15;
		// This convar can be changed in competitive (strict) settings mode even though it is not archived. Meant for one-offs like cl_showfps that are not user facing settings but are benign
		static constexpr uint32_t AllowInComp 			= 1 << 18;


		static constexpr uint32_t MatThreadMask = ReloadMaterials | ReloadTextures | MatSysThread;
	}
}

class IConCommandBaseAccessor
{
public:
	virtual bool RegisterConCommandBase(ConCommandBase* pVar) abstract;
};

class ConVar_Internal
{
public:
	virtual void SetValue(const char* pValue) abstract;
	virtual void SetValue(float flValue) abstract;
	virtual void SetValue(int nValue) abstract;

	virtual const char* GetName() const abstract;

	virtual bool IsFlagSet(int nFlag) const abstract;
};

class ConCommandBase
{
	friend class CCvar;
	friend class ConVar;
	friend class ConCommand;
public:
	ConCommandBase() = default;
	ConCommandBase(const char* pName, const char* pHelpString = nullptr, uint32_t flags = Const::ConvarFlags::None)
	{
		CreateBase(pName, pHelpString, flags);
	}

	virtual ~ConCommandBase() = default;

	virtual	bool IsCommand() const noexcept { return true; }

	virtual bool IsFlagSet(uint32_t flag) const noexcept { return (Flags & flag) == flag; }
	virtual void AddFlags(uint32_t flags) noexcept { Flags |= flags; }

	virtual const char* GetName() const noexcept { return Name; }

	virtual const char* GetHelpText() const noexcept { return HelpString; }

	const ConCommandBase* GetNext() const noexcept { return NextBase; }
	ConCommandBase* GetNext() noexcept { return NextBase; }

	virtual bool IsRegistered() const noexcept { return Is_Registered; }

	PX_SDK_TF2 virtual 
		VCVarDLLIdentifier GetDLLIdentifier() const;

	PX_SDK_TF2 static void 
		Register(uint32_t nCVarFlag = 0, IConCommandBaseAccessor* pAccessor = nullptr);
	PX_SDK_TF2 static void
		Unregister();

	ConCommandBase(const ConCommandBase&) = default;
	ConCommandBase& operator=(const ConCommandBase&) = default;
	ConCommandBase(ConCommandBase&&) = default;
	ConCommandBase& operator=(ConCommandBase&&) = default;

protected:
	PX_SDK_TF2 virtual void 
		CreateBase(const char* pName, const char* pHelpString = nullptr, uint32_t flags = Const::ConvarFlags::None);

	// Used internally by OneTimeInit to initialize/shutdown
	virtual void Init()
	{
		if (Accessor)
			Accessor->RegisterConCommandBase(this);
	}
	void Shutdown();

	// Internal copy routine ( uses new operator from correct module )
	std::string CopyString(const char* from);

public:
	// Next ConVar in chain
	// Prior to register, it points to the next convar in the DLL.
	// Once registered, though, m_pNext is reset to point to the next
	// convar in the global list
	ConCommandBase* NextBase{ };

	// Has the cvar been added to the global list?
	bool Is_Registered{ };

	// Static data
	const char* Name{ };
	const char* HelpString{ };

	// ConVar flags
	uint32_t Flags{ };

protected:
	// ConVars add themselves to this list for the executable. 
	// Then ConVar_Register runs through  all the console variables 
	// and registers them into a global list stored in vstdlib.dll
	static inline ConCommandBase* ConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static inline IConCommandBaseAccessor* Accessor;
};

TF2_NAMESPACE_END();
