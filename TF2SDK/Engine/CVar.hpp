#pragma once

#include "AppSystem.hpp"
#include "ConVar_Internal.hpp"
#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();

class ConCommandBase;
class ConVar;
class ConCommand;
class IConsoleDisplayFunc;
class ICvarQuery;

class ICVar : public IAppSystem
{
public:
	virtual VCVarDLLIdentifier AllocateDLLIdentifier() abstract;

	virtual void RegisterConCommand(ConCommandBase* pCommandBase) abstract;
	virtual void UnregisterConCommand(ConCommandBase* pCommandBase) abstract;
	virtual void UnregisterConCommands(VCVarDLLIdentifier id) abstract;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char* GetCommandLineValue(const char* pVariableName) abstract;

	virtual ConCommandBase* FindCommandBase(const char* name) abstract;
	virtual const ConCommandBase* FindCommandBase(const char* name) const abstract;
	virtual ConVar* FindVar(const char* var_name) abstract;
	virtual const ConVar* FindVar(const char* var_name) const abstract;
	virtual ConCommand* FindCommand(const char* name) abstract;
	virtual const ConCommand* FindCommand(const char* name) const abstract;

	// Get first ConCommandBase to allow iteration
	virtual ConCommandBase* GetCommands() abstract;
	virtual const ConCommandBase* GetCommands() const abstract;

	// Install a global change callback (to be called when any convar changes) 
	virtual void			InstallGlobalChangeCallback(ConVarChangeCallback callback) abstract;
	virtual void			RemoveGlobalChangeCallback(ConVarChangeCallback callback) abstract;
	virtual void			CallGlobalChangeCallbacks(ConVar* var, const char* pOldString, float flOldValue) abstract;

	// Install a console printer
	virtual void			InstallConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) abstract;
	virtual void			RemoveConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) abstract;
	virtual void			ConsoleColorPrintf(const Color3_8& clr, const char* pFormat, ...) const abstract;
	virtual void			ConsolePrintf(const char* pFormat, ...) const abstract;
	virtual void			ConsoleDPrintf(const char* pFormat, ...) const abstract;

	// Reverts cvars which contain a specific flag
	virtual void			RevertFlaggedConVars(uint32_t nFlag) abstract;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICVar, so we can't use the standard connect pattern
	virtual void			InstallCVarQuery(ICvarQuery* pQuery) abstract;

	virtual bool			IsMaterialThreadSetAllowed() const abstract;
	virtual void			QueueMaterialThreadSetValue(ConVar* pConVar, const char* pValue) abstract;
	virtual void			QueueMaterialThreadSetValue(ConVar* pConVar, int nValue) abstract;
	virtual void			QueueMaterialThreadSetValue(ConVar* pConVar, float flValue) abstract;
	virtual bool			HasQueuedMaterialThreadConVarSets() const abstract;
	virtual int				ProcessQueuedMaterialThreadConVarSets() abstract;

protected:	class ICVarIteratorInternal;
public:
	/// Iteration over all cvars. 
	/// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
	/// usage: 
	/// { ICvar::Iterator iter(g_pCVar); 
	///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	///   {  
	///       ConCommandBase *cmd = iter.Get();
	///   } 
	/// }
	/// The Iterator class actually wraps the internal factory methods
	/// so you don't need to worry about new/delete -- scope takes care
	//  of it.
	/// We need an iterator like this because we can't simply return a 
	/// pointer to the internal data type that contains the cvars -- 
	/// it's a custom, protected class with unusual semantics and is
	/// prone to change.
	class Iterator
	{
	public:
		inline Iterator(ICVar* icvar);
		inline ~Iterator();
		inline void		SetFirst();
		inline void		Next();
		inline bool		IsValid();
		inline ConCommandBase* Get();
	private:
		ICVarIteratorInternal* Iter;
	};

protected:
	// internals for  ICVarIterator
	class ICVarIteratorInternal
	{
	public:
		// warning: delete called on 'ICvar::ICVarIteratorInternal' that is abstract but has non-virtual destructor [-Wdelete-non-virtual-dtor]
		virtual ~ICVarIteratorInternal() = default;
		virtual void		SetFirst() abstract;
		virtual void		Next() abstract;
		virtual	bool		IsValid() abstract;
		virtual ConCommandBase* Get() abstract;

	public:
		ICVarIteratorInternal(const ICVarIteratorInternal&) = default;
		ICVarIteratorInternal& operator=(const ICVarIteratorInternal&) = default;
		ICVarIteratorInternal(ICVarIteratorInternal&&) = default;
		ICVarIteratorInternal& operator=(ICVarIteratorInternal&&) = default;
	};

	virtual ICVarIteratorInternal* FactoryInternalIterator() abstract;
	friend class Iterator;
};

inline ICVar::Iterator::Iterator(ICVar* icvar)
{
	Iter = icvar->FactoryInternalIterator();
}

inline ICVar::Iterator::~Iterator()
{
	delete Iter;
}

inline void ICVar::Iterator::SetFirst()
{
	Iter->SetFirst();
}

inline void ICVar::Iterator::Next()
{
	Iter->Next();
}

inline bool ICVar::Iterator::IsValid()
{
	return Iter->IsValid();
}

inline ConCommandBase* ICVar::Iterator::Get()
{
	return Iter->Get();
}


TF2_NAMESPACE_END();
