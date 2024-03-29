#pragma once

#include <tf2/math/Vector.hpp>
#include <tf2/math/Matrix.hpp>
#include <tf2/consts.hpp>

TF2_NAMESPACE_BEGIN();

class IBaseHandle;
class IClientNetworkable;
class IClientUnknown;
class IBaseEntityInternal;
class IClientEntityList;


namespace interfaces
{
	TF2_EXPORT_INTERFACE(IClientEntityList, ClientList, "VClientEntityList003");
}


class IClientEntityList
{
public:
	// Get IClientNetworkable interface for specified entity
	virtual IClientNetworkable* GetClientNetworkable(int entnum) abstract;
	virtual IClientNetworkable* GetClientNetworkableFromHandle(IBaseHandle hEnt) abstract;
	virtual IClientUnknown* GetClientUnknownFromHandle(IBaseHandle hEnt) abstract;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual IBaseEntityInternal* GetClientEntity(int entnum) abstract;
	virtual IBaseEntityInternal* GetClientEntityFromHandle(IBaseHandle hEnt) abstract;

	// Returns number of entities currently in use
	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) abstract;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex() abstract;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities(int maxents) abstract;
	virtual int					GetMaxEntities() abstract;
};

class IClientEntityListener
{
public:
	virtual void OnEntityCreated(IBaseEntityInternal* pEnt) { };
	virtual void OnEntityDeleted(IBaseEntityInternal* pEnt) { };

public:
	PX_SDK_TF2 void AddEntityListener() noexcept;
	PX_SDK_TF2 void RemoveEntityListener() noexcept;
};

TF2_NAMESPACE_END();
