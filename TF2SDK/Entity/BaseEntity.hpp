#pragma once

#include <Interfaces/GameData.hpp>

#include "ClientEntity.hpp"
#include "HandleEntity.hpp"

#include "GameProp/GameProp.hpp"
#include "TF2Consts.hpp"

#include "Math/Vector.hpp"


TF2_NAMESPACE_BEGIN();

class IGlowObject;
class IBoneCache;
class IBoneInfo;
struct BoneResult;
class ITFParticleFactory;
class IPhysicsObject;

class IBaseEntityInternal : public IClientEntity
{
public:
	/// <summary>
	/// Get entity from current index
	/// </summary>
	/// <returns>an entity pointer, nullptr if the entity doesn't exists</returns>
	SG_SDK_TF2 static IBaseEntityInternal*
		GetEntity(int ent_index);

	/// <summary>
	/// Get entity from it's handle
	/// </summary>
	/// <returns>an entity pointer, nullptr if the entity doesn't exists</returns>
	SG_SDK_TF2 static IBaseEntityInternal*
		GetEntity(IBaseHandle ent_handle);

	/// <summary>
	/// Get local client's entity pointer
	/// Must be used in-game, otherwise it's UB
	/// </summary>
	/// <returns>an entity pointer, nullptr if the entity doesn't exists</returns>
	SG_SDK_TF2 static IBaseEntityInternal*
		GetLocalPlayer();

	/// <summary>
	/// Get highest entity index slot
	/// Must be used in-game, otherwise it's UB
	/// </summary>
	SG_SDK_TF2 static int
		GetHighestEntityIndex();

	/// <summary>
	/// Test if the current entity is valid (not null and not dormant)
	/// </summary>
	static bool	BadEntity(IBaseEntityInternal* ent_ptr)
	{
		return !ent_ptr || ent_ptr->IsDormant();
	}

	/// <summary>
	/// Test if the local client is valid (in game and not a bad entity)
	/// </summary>
	SG_SDK_TF2 static bool BadLocal();

	/// <summary>
	/// Tests whether the entity is derived from CBaseCombatWeapon or not
	/// </summary>
	SG_SDK_TF2 bool
		IsBaseCombatWeapon() const noexcept;

	/// <summary>
	/// </summary>
	/// <param name="is_pred">true to call GetPredDescMap, false to call GetDataDescMap</param>
	SG_SDK_TF2 EntityDataMap*
		GetDataMap(bool is_pred) const noexcept;

	/// <summary>
	/// Get entity's bone cache by calling 'CBaseEntity::GetBoneCache'
	/// </summary>
	/// <returns></returns>
	SG_SDK_TF2 const IBoneCache*
		GetBoneCache() const;

	/// <summary>
	/// Get entity bone info { bone cache, studio hitbox, studio hdr }
	/// </summary>
	SG_SDK_TF2 bool
		QueryBoneInfo(IBoneInfo&) const;

	/// <summary>
	/// Get entity's bone position from cache and output it to results on success
	/// </summary>
	SG_SDK_TF2 bool
		GetBonePosition(int bone_position, BoneResult& results) const;

	/// <summary>
	/// Sets entity's model by index by calling 'CBaseEntity::SetModelIndex'
	/// </summary>
	/// <param name="modelidx"></param>
	SG_SDK_TF2 void
		SetModel(int modelidx);

	/// <summary>
	/// Tests whether the entity is health kit
	/// </summary>
	/// <returns></returns>
	SG_SDK_TF2 bool
		IsHealthKit() const noexcept;

	/// <summary>
	/// Tests whether the entity is a ghost
	/// </summary>
	/// <returns></returns>
	SG_SDK_TF2 bool
		IsGhost() const noexcept;

	/// <summary>
	/// Get either local velocity or predicted velocity
	/// </summary>
	SG_SDK_TF2 void 
		EstimateAbsVelocity(Vector3D_F& vel);
	
	/// <summary>
	/// Tests whether the entity is class id
	/// </summary>
	bool IsClassID(Const::EntClassID id) const
	{
		return this->GetClientClass()->ClassID == id;
	}

	SG_SDK_TF2 int
		LookupAttachment(const char* name);

	bool GetAttachment(const char* name, Vector3D_F& origin, Angle_F& angles)
	{
		return GetAttachment(LookupAttachment(name), origin, angles);
	}

	SG_SDK_TF2 bool
		GetAttachment(int attach_pt, Vector3D_F& origin, Angle_F& angles);


#define DECL_RECVPROP(Type, Prop_Name, Custom_Name, Extra_Offset)	SG_DECL_RECVPROP(IBaseEntityInternal, Type, Const::EntClassID::CBaseAnimating, Prop_Name, Custom_Name, Extra_Offset)

	DECL_RECVPROP(int,					"m_nModelIndex",		ModelIndex, 0);
	DECL_RECVPROP(Vector3D_F,			"m_vecOrigin",			VecOrigin, 0);
	DECL_RECVPROP(Vector3D_F,			"m_vecForce",			VecForce, 0);
	DECL_RECVPROP(Angle_F,				"m_angRotation",		AngRotation, 0);

	DECL_RECVPROP(ICollideable,				"m_Collision",			CollisionProp, 0);
	DECL_RECVPROP(Vector3D_F,				"m_vecMinsPreScaled",	MinsPrescaled, 0);
	DECL_RECVPROP(Vector3D_F,				"m_vecMaxsPreScaled",	MaxsPrescaled, 0);
	DECL_RECVPROP(Vector3D_F,				"m_vecMins",			Mins, 0);
	DECL_RECVPROP(Vector3D_F,				"m_vecMaxs",			Maxs, 0);
	DECL_RECVPROP(Const::EntSolidType,		"m_nSolidType",			SolidType, 0);
	DECL_RECVPROP(uint32_t,					"m_usSolidFlags",		SolidFlags, 0);
	DECL_RECVPROP(Const::EntCollisionGroup,	"m_CollisionGroup",		CollisionGroup, 0);

	DECL_RECVPROP(float,				"m_flSimulationTime",	SimulationTime, 0);
	DECL_RECVPROP(float,				"m_flAnimTime",			AnimationTime, 0);
	DECL_RECVPROP(float,				"m_flCycle",			Cycle, 0);
	DECL_RECVPROP(float,				"m_flModelScale",		ModelScale, 0);
	DECL_RECVPROP(float,				"m_flModelWidthScale",	ModelWidthScale, 0);

	DECL_RECVPROP(Const::TFTeam,		"m_iTeamNum",			TeamNum, 0);
	DECL_RECVPROP(IBaseHandle,			"m_hOwnerEntity",		OwnerEntity, 0);
	DECL_RECVPROP(int,					"m_nModelIndexOverride",ModelIndexOverride, 0);
	DECL_RECVPROP(int,					"m_nHitboxSet",			HitboxSet, 0);
	DECL_RECVPROP(int,					"m_nSequence",			Sequence, 0);
	DECL_RECVPROP(int,					"m_nForceBone",			ForceBone, 0);
	DECL_RECVPROP(int,					"m_nSkin",				Skin, 0);
	DECL_RECVPROP(int,					"m_nBody",				Body, 0);

	DECL_RECVPROP(ITFParticleFactory,	"m_flElasticity",		ParticleProp, 
		[]() -> int
		{
			return Interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "offsets" }, "m_flElasticity__To__m_ParticleProp").value_or(0);
		}()
	);
	DECL_RECVPROP(IPhysicsObject,		"m_iTeamNum",			PhysicsObject,
		[]() -> int
		{
			return Interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "offsets" }, "m_iTeamNum__To__m_pPhysicsObject").value_or(0);
		}()
	);

	struct Fpp4 {
		static IBaseEntityInternal* f(IBaseEntityInternal* pent)
		{
			return pent;
	}
		static void oo()
		{
		}
	};

	SG_DECL_DATAMAP(IBaseEntityInternal,	Const::EntMoveType,	true,		"m_MoveType",	MoveType, 0);
	SG_DECL_DATAMAP(IBaseEntityInternal,	uint32_t,			true,		"m_iEFlags",	EntFlags, 0);

#undef DECL_RECVPROP
};


template<class EntityClassType>
class IBaseEntityWrapper
{
	template<typename OtherType>
	using IsBaseEntity = std::enable_if_t<std::is_base_of_v<IBaseEntityInternal, OtherType>>;

public:
	IBaseEntityWrapper(int index)							noexcept { set(EntityClassType::GetEntity(index)); };

	IBaseEntityWrapper(const IBaseHandle& hndl)				noexcept { set(EntityClassType::GetEntity(hndl)); };

	IBaseEntityWrapper(EntityClassType* ptr)				noexcept { set(ptr); };
	IBaseEntityWrapper(const EntityClassType* ptr)			noexcept { set(ptr); };
	IBaseEntityWrapper(void* ptr)							noexcept { set(reinterpret_cast<EntityClassType*>(ptr)); };
	IBaseEntityWrapper<EntityClassType>& operator=(void* p) noexcept { set(reinterpret_cast<EntityClassType*>(p)); return *this; }

	IBaseEntityWrapper(nullptr_t)							noexcept { set(nullptr); }

	IBaseEntityWrapper()									= default;
	IBaseEntityWrapper(const IBaseEntityWrapper&)			= default;
	IBaseEntityWrapper& operator=(const IBaseEntityWrapper&)= default;
	IBaseEntityWrapper(IBaseEntityWrapper&&)				= default;
	IBaseEntityWrapper& operator=(IBaseEntityWrapper&&)		= default;

	template<typename OtherType, typename = IsBaseEntity<OtherType>>
	IBaseEntityWrapper(const IBaseEntityWrapper<OtherType>& o)				noexcept { set((EntityClassType*)o.get()); }
	template<typename OtherType, typename = IsBaseEntity<OtherType>>
	IBaseEntityWrapper& operator=(const IBaseEntityWrapper<OtherType>& o)	noexcept { set((EntityClassType*)o.get()); return *this; }
	template<typename OtherType, typename = IsBaseEntity<OtherType>>
	IBaseEntityWrapper(IBaseEntityWrapper<OtherType>&& o)					noexcept { set((EntityClassType*)o.get()); o.set(nullptr); }
	template<typename OtherType, typename = IsBaseEntity<OtherType>>
	IBaseEntityWrapper& operator=(IBaseEntityWrapper<OtherType>&& o)		noexcept { set((EntityClassType*)o.get()); o.set(nullptr); return *this; }

public:
	const EntityClassType* operator->() const noexcept
	{
		return Entity;
	}

	EntityClassType* operator->() noexcept
	{
		return Entity;
	}

	const EntityClassType* get() const noexcept
	{
		return Entity;
	}

	EntityClassType* get() noexcept
	{
		return Entity;
	}

	void set(EntityClassType* ent) noexcept
	{
		Entity = ent;
	}

	operator bool() const noexcept
	{
		return !EntityClassType::BadEntity(Entity);
	}

	bool operator==(const EntityClassType* other) const noexcept { return Entity == other; }

	bool operator==(const IBaseEntityWrapper&) const = default;

protected:
	EntityClassType* Entity{ nullptr };
};

using IBaseEntity = IBaseEntityWrapper<IBaseEntityInternal>;

TF2_NAMESPACE_END();