#pragma once

#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN();

class INetworkStringTableContainer;
class INetworkStringTable;


namespace Interfaces
{
	TF2_EXPORT_INTERFACE(INetworkStringTableContainer, NSTContainer, "VEngineClientStringTable001");
}

namespace Const
{
	using NSTTableID = int;
	using NSTChangedCallback = void(*)(void* object, INetworkStringTable*, NSTTableID stringnum, const char* newstring, const void* newdata);

	static constexpr NSTTableID InvalidNSTIndex = -1;
	static constexpr size_t NSTMaxtTables = 32;

	enum class NSTTableSide : bool
	{
		Client = false,
		Server
	};
}

class INetworkStringTable
{
public:

	virtual					~INetworkStringTable() = default;

	// Table Info
	virtual const char* GetTableName() const abstract;
	virtual Const::NSTTableID GetTableId() const abstract;
	virtual int				GetNumStrings() const abstract;
	virtual int				GetMaxStrings() const abstract;
	virtual int				GetEntryBits() const abstract;

	// Networking
	virtual void			SetTick(int tick) abstract;
	virtual bool			ChangedSinceTick(int tick) const abstract;

	// Accessors (length -1 means don't change user data if string already exits)
	virtual int				AddString(Const::NSTTableSide type, const char* value, int length = -1, const void* userdata = nullptr) abstract;

	virtual const char* GetString(int stringNumber) abstract;
	virtual void			SetStringUserData(int stringNumber, int length, const void* userdata) abstract;
	virtual const void* GetStringUserData(int stringNumber, int* length) abstract;
	virtual int				FindStringIndex(char const* string) abstract; // returns INVALID_NST_INDEX if not found

	// Callbacks
	virtual void			SetStringChangedCallback(void* object, Const::NSTChangedCallback changeFunc) abstract;
};

class INetworkStringTableContainer
{
public:
	virtual					~INetworkStringTableContainer() = default;

	// table creation/destruction
	virtual INetworkStringTable* CreateStringTable(const char* tableName, int maxentries, int userdatafixedsize = 0, int userdatanetworkbits = 0) abstract;
	virtual void				RemoveAllTables() abstract;

	// table infos
	virtual INetworkStringTable* FindTable(const char* tableName) const abstract;
	virtual INetworkStringTable* GetTable(Const::NSTTableID stringTable) const abstract;
	virtual int					GetNumTables() const abstract;

	virtual INetworkStringTable* CreateStringTableEx(const char* tableName, int maxentries, int userdatafixedsize = 0, int userdatanetworkbits = 0, bool bIsFilenames = false) abstract;
	virtual void				SetAllowClientSideAddString(INetworkStringTable* table, bool bAllowClientSideAddString) abstract;
};

TF2_NAMESPACE_END();