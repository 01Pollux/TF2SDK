#pragma once

#include <px/interfaces/GameData.hpp>

#include <tf2/gameprop/GameProp.hpp>
#include <tf2/consts.hpp>

#include <tf2/math/Vector.hpp>

#include "ClientEntity.hpp"
#include "HandleEntity.hpp"

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
	[[nodiscard]] PX_SDK_TF2 static IBaseEntityInternal*
		GetEntity(int ent_index);

	/// <summary>
	/// Get entity from it's handle
	/// </summary>
	/// <returns>an entity pointer, nullptr if the entity doesn't exists</returns>
	[[nodiscard]] PX_SDK_TF2 static IBaseEntityInternal*
		GetEntity(IBaseHandle ent_handle);

	/// <summary>
	/// Get local client's entity pointer
	/// Must be used in-game, otherwise it's UB
	/// </summary>
	/// <returns>an entity pointer, nullptr if the entity doesn't exists</returns>
	[[nodiscard]] PX_SDK_TF2 static IBaseEntityInternal*
		GetLocalPlayer();

	/// <summary>
	/// Get highest entity index slot
	/// Must be used in-game, otherwise it's UB
	/// </summary>
	[[nodiscard]] PX_SDK_TF2 static int
		GetHighestEntityIndex();

	/// <summary>
	/// Test if the current entity is valid (not null and not dormant)
	/// </summary>
	[[nodiscard]] static bool 
		BadEntity(IBaseEntityInternal* ent_ptr)
	{
		return !ent_ptr || ent_ptr->IsDormantEx();
	}

	/// <summary>
	/// Test if the local client is valid (in game and not a bad entity)
	/// </summary>
	[[nodiscard]] PX_SDK_TF2 static bool
		BadLocal();

	/// <summary>
	/// Tests whether the entity is derived from CBaseCombatWeapon or not
	/// </summary>
	[[nodiscard]] PX_SDK_TF2 bool
		IsBaseCombatWeapon() const noexcept;

	/// <summary>
	/// </summary>
	/// <param name="is_pred">true to call GetPredDescMap, false to call GetDataDescMap</param>
	[[nodiscard]] PX_SDK_TF2 EntityDataMap*
		GetDataMap(bool is_pred) const noexcept;

	/// <summary>
	/// Get entity's bone cache by calling 'CBaseEntity::GetBoneCache'
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] PX_SDK_TF2 const IBoneCache*
		GetBoneCache() const;

	/// <summary>
	/// Get entity bone info { bone cache, studio hitbox, studio hdr }
	/// </summary>
	[[nodiscard]] PX_SDK_TF2 bool
		QueryBoneInfo(IBoneInfo&) const;

	/// <summary>
	/// Get entity's bone position from cache and output it to results on success
	/// </summary>
	[[nodiscard]] PX_SDK_TF2 bool
		GetBonePosition(int bone_position, BoneResult& results) const;

	/// <summary>
	/// Sets entity's model by index by calling 'CBaseEntity::SetModelIndex'
	/// </summary>
	/// <param name="modelidx"></param>
	[[nodiscard]] PX_SDK_TF2 void
		SetModel(int modelidx);

	/// <summary>
	/// Tests whether the entity is health kit
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] PX_SDK_TF2 bool
		IsHealthKit() const noexcept;

	/// <summary>
	/// Tests whether the entity is a ghost
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] PX_SDK_TF2 bool
		IsGhost() const noexcept;

	/// <summary>
	/// Get either local velocity or predicted velocity
	/// </summary>
	PX_SDK_TF2 void
		EstimateAbsVelocity(Vector3D_F& vel) const;

	/// <summary>
	/// Tests whether the entity is class id
	/// </summary>
	[[nodiscard]] bool 
		IsClassID(Const::EntClassID id) const
	{
		return this->GetClientClass()->ClassID == id;
	}

	/// <summary>
	/// Get attachement's index by name
	/// </summary>
	/// <returns>-1 if the attachement doesn't exists</returns>
	[[nodiscard]] int LookupAttachment(const char* name) const
	{
		return this->GetClientRenderable()->LookupAttachment(name);
	}

	/// <summary>
	/// Get attachement's original and angle by name
	/// </summary>
	/// <returns>false if the attachement doesn't exists</returns>
	[[nodiscard]] bool GetAttachment(const char* name, Vector3D_F& origin, Angle_F& angles) const
	{
		IClientRenderable* pRender = this->GetClientRenderable();
		return pRender->GetAttachment(pRender->LookupAttachment(name), origin, angles);
	}

	/// <summary>
	/// Get attachement's original and angle by index
	/// </summary>
	/// <returns>false if the attachement doesn't exists</returns>
	[[nodiscard]] bool GetAttachment(const char* name, Matrix3x4_F& matrix) const
	{
		IClientRenderable* pRender = this->GetClientRenderable();
		return pRender->GetAttachment(pRender->LookupAttachment(name), matrix);
	}

	/// <summary>
	/// Get attachement's original and angle by name
	/// </summary>
	/// <returns>false if the attachement doesn't exists</returns>
	[[nodiscard]] bool GetAttachment(int attach_pt, Vector3D_F& origin, Angle_F& angles) const
	{
		IClientRenderable* pRender = this->GetClientRenderable();
		return pRender->GetAttachment(attach_pt, origin, angles);
	}

	/// <summary>
	/// Get attachement's original and angle by index
	/// </summary>
	/// <returns>false if the attachement doesn't exists</returns>
	[[nodiscard]] bool GetAttachment(int attach_pt, Matrix3x4_F& matrix) const
	{
		IClientRenderable* pRender = this->GetClientRenderable();
		return pRender->GetAttachment(attach_pt, matrix);
	}
	
	/// <summary>
	/// Test if the entity should collide with current collision properties
	/// </summary>
	[[nodiscard]] PX_SDK_TF2 bool
		ShouldCollide(Const::EntCollisionGroup groups, uint32_t mask);

	/// <summary>
	/// Get entity's entity
	/// this is the same as entindex, except it's not virtual
	/// </summary>
	/// <returns>-1 if the entity wasn't networkable</returns>
	[[nodiscard]] int
		GetEntIndex() const noexcept
	{
		return m_EntIndex;
	}

	/// <summary>
	/// Checks if the entity is dormant
	/// this is the same as IsDormant, except it's not virtual
	/// </summary>
	[[nodiscard]] bool
		IsDormantEx() const noexcept
	{
		return GetEntIndex() != -1 ? m_IsDormant : false;
	}

#define DECL_RECVPROP(Type, Prop_Name, Custom_Name, Extra_Offset)	PX_DECL_RECVPROP(IBaseEntityInternal, Type, Const::EntClassID::CBaseAnimating, Prop_Name, Custom_Name, Extra_Offset)

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
			return interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "offsets" }, "m_flElasticity__To__m_ParticleProp").value_or(0);
		}()
	);
	DECL_RECVPROP(IPhysicsObject,		"m_iTeamNum",			PhysicsObject,
		[]() -> int
		{
			return interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "offsets" }, "m_iTeamNum__To__m_pPhysicsObject").value_or(0);
		}()
	);
	PX_DECL_RECVPROP(IBaseEntityInternal,	uint32_t,			Const::EntClassID::CBaseAnimating, "m_fFlags", EntFlags, 0);

	PX_DECL_DATAMAP(IBaseEntityInternal,	Const::EntMoveType,	true,		"m_MoveType",	MoveType, 0);
	
private:
	DECL_RECVPROP(int,					"m_nRenderFX",			m_EntIndex,
		[]() -> int
		{
			return interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "offsets" }, "m_nRenderFX__To__m_iEntityIndex").value_or(0);
		}()
	);
	
	DECL_RECVPROP(bool,					"m_iParentAttachment",	m_IsDormant,
		[]() -> int
		{
			return interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "offsets" }, "m_iParentAttachment__To__m_bDormant").value_or(0);
		}()
	);
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

	[[nodiscard]] const EntityClassType* get() const noexcept
	{
		return Entity;
	}

	[[nodiscard]] EntityClassType* get() noexcept
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

	[[nodiscard]] bool operator==(const EntityClassType* other) const noexcept { return Entity == other; }

	[[nodiscard]] bool operator==(const IBaseEntityWrapper&) const = default;

protected:
	EntityClassType* Entity{ nullptr };
};

using IBaseEntity = IBaseEntityWrapper<IBaseEntityInternal>;

TF2_NAMESPACE_END();