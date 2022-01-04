#pragma once

#include <tf2/math/Vector.hpp>
#include <tf2/engine/AppSystem.hpp>

TF2_NAMESPACE_BEGIN();

class IServerTools;
class KeyValues;
class IServerEntity;
class IClientEntity;
class ISBaseEntity;
class CTakeDamageInfo;
class ITempEntsSystem;
class ISBaseTempEntity;
class IEntityFactoryDictionary;
class ISEntityList;
class IEntityFindFilter;

struct FlashlightState_t;

namespace interfaces
{
	TF2_EXPORT_INTERFACE(IServerTools, ServerTools, "VSERVERTOOLS003");
}

class IServerTools : public IBaseInterface
{
public:
	virtual IServerEntity* GetIServerEntity(IClientEntity* pClientEntity) abstract;
	virtual bool SnapPlayerToPosition(const Vector3D_F& org, const Angle_F& ang, IClientEntity* pClientPlayer = nullptr) abstract;
	virtual bool GetPlayerPosition(Vector3D_F& org, Angle_F& ang, IClientEntity* pClientPlayer = nullptr) abstract;
	virtual bool SetPlayerFOV(int fov, IClientEntity* pClientPlayer = nullptr) abstract;
	virtual int GetPlayerFOV(IClientEntity* pClientPlayer = nullptr) abstract;
	virtual bool IsInNoClipMode(IClientEntity* pClientPlayer = nullptr) abstract;

	// entity searching
	virtual ISBaseEntity* FirstEntity() abstract;
	virtual ISBaseEntity* NextEntity(ISBaseEntity* pEntity) abstract;
	virtual ISBaseEntity* FindEntityByHammerID(int iHammerID) abstract;

	// entity query
	virtual bool GetKeyValue(ISBaseEntity* pEntity, const char* szField, char* szValue, int iMaxLen) abstract;
	virtual bool SetKeyValue(ISBaseEntity* pEntity, const char* szField, const char* szValue) abstract;
	virtual bool SetKeyValue(ISBaseEntity* pEntity, const char* szField, float flValue) abstract;
	virtual bool SetKeyValue(ISBaseEntity* pEntity, const char* szField, const Vector3D_F& vecValue) abstract;

	// entity spawning
	virtual ISBaseEntity* CreateEntityByName(const char* szClassName) abstract;
	virtual void DispatchSpawn(ISBaseEntity* pEntity) abstract;

	// This reloads a portion or all of a particle definition file.
	// It's up to the server to decide if it cares about this file
	// Use a UtlBuffer to crack the data
	virtual void ReloadParticleDefintions(const char* pFileName, const void* pBufData, int nLen) abstract;

	virtual void AddOriginToPVS(const Vector3D_F& org) abstract;
	virtual void MoveEngineViewTo(const Vector3D_F& vPos, const Angle_F& vAngles) abstract;

	virtual bool DestroyEntityByHammerId(int iHammerID) abstract;
	virtual ISBaseEntity* GetBaseEntityByEntIndex(int iEntIndex) abstract;
	virtual void RemoveEntity(ISBaseEntity* pEntity) abstract;
	virtual void RemoveEntityImmediate(ISBaseEntity* pEntity) abstract;
	virtual IEntityFactoryDictionary* GetEntityFactoryDictionary(void) abstract;

	virtual void SetMoveType(ISBaseEntity* pEntity, int val) abstract;
	virtual void SetMoveType(ISBaseEntity* pEntity, int val, int moveCollide) abstract;
	virtual void ResetSequence(ISBaseEntity* pEntity, int nSequence) abstract;
	virtual void ResetSequenceInfo(ISBaseEntity* pEntity) abstract;

	virtual void ClearMultiDamage() abstract;
	virtual void ApplyMultiDamage() abstract;
	virtual void AddMultiDamage(const CTakeDamageInfo& pTakeDamageInfo, ISBaseEntity* pEntity) abstract;
	virtual void RadiusDamage(const CTakeDamageInfo& info, const Vector3D_F& vecSrc, float flRadius, int iClassIgnore, ISBaseEntity* pEntityIgnore) abstract;

	virtual ITempEntsSystem* GetTempEntsSystem() abstract;
	virtual ISBaseTempEntity* GetTempEntList() abstract;

	virtual ISEntityList* GetEntityList() abstract;
	virtual bool IsEntityPtr(void* pTest) abstract;
	virtual ISBaseEntity* FindEntityByClassname(ISBaseEntity* pStartEntity, const char* szName) abstract;
	virtual ISBaseEntity* FindEntityByName(ISBaseEntity* pStartEntity, const char* szName, ISBaseEntity* pSearchingEntity = nullptr, ISBaseEntity* pActivator = nullptr, ISBaseEntity* pCaller = nullptr, IEntityFindFilter* pFilter = nullptr) abstract;
	virtual ISBaseEntity* FindEntityInSphere(ISBaseEntity* pStartEntity, const Vector3D_F& vecCenter, float flRadius) abstract;
	virtual ISBaseEntity* FindEntityByTarget(ISBaseEntity* pStartEntity, const char* szName) abstract;
	virtual ISBaseEntity* FindEntityByModel(ISBaseEntity* pStartEntity, const char* szModelName) abstract;
	virtual ISBaseEntity* FindEntityByNameNearest(const char* szName, const Vector3D_F& vecSrc, float flRadius, ISBaseEntity* pSearchingEntity = nullptr, ISBaseEntity* pActivator = nullptr, ISBaseEntity* pCaller = nullptr) abstract;
	virtual ISBaseEntity* FindEntityByNameWithin(ISBaseEntity* pStartEntity, const char* szName, const Vector3D_F& vecSrc, float flRadius, ISBaseEntity* pSearchingEntity = nullptr, ISBaseEntity* pActivator = nullptr, ISBaseEntity* pCaller = nullptr) abstract;
	virtual ISBaseEntity* FindEntityByClassnameNearest(const char* szName, const Vector3D_F& vecSrc, float flRadius) abstract;
	virtual ISBaseEntity* FindEntityByClassnameWithin(ISBaseEntity* pStartEntity, const char* szName, const Vector3D_F& vecSrc, float flRadius) abstract;
	virtual ISBaseEntity* FindEntityByClassnameWithin(ISBaseEntity* pStartEntity, const char* szName, const Vector3D_F& vecMins, const Vector3D_F& vecMaxs) abstract;
	virtual ISBaseEntity* FindEntityGeneric(ISBaseEntity* pStartEntity, const char* szName, ISBaseEntity* pSearchingEntity = nullptr, ISBaseEntity* pActivator = nullptr, ISBaseEntity* pCaller = nullptr) abstract;
	virtual ISBaseEntity* FindEntityGenericWithin(ISBaseEntity* pStartEntity, const char* szName, const Vector3D_F& vecSrc, float flRadius, ISBaseEntity* pSearchingEntity = nullptr, ISBaseEntity* pActivator = nullptr, ISBaseEntity* pCaller = nullptr) abstract;
	virtual ISBaseEntity* FindEntityGenericNearest(const char* szName, const Vector3D_F& vecSrc, float flRadius, ISBaseEntity* pSearchingEntity = nullptr, ISBaseEntity* pActivator = nullptr, ISBaseEntity* pCaller = nullptr) abstract;
	virtual ISBaseEntity* FindEntityNearestFacing(const Vector3D_F& origin, const Vector3D_F& facing, float threshold) abstract;
	virtual ISBaseEntity* FindEntityClassNearestFacing(const Vector3D_F& origin, const Vector3D_F& facing, float threshold, char* classname) abstract;
	virtual ISBaseEntity* FindEntityProcedural(const char* szName, ISBaseEntity* pSearchingEntity = nullptr, ISBaseEntity* pActivator = nullptr, ISBaseEntity* pCaller = nullptr) abstract;
};


TF2_NAMESPACE_END();
