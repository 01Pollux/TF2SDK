
#include <shadowgarden/interfaces/GameData.hpp>
#include <tf2/utils/Thunks.hpp>

#include <tf2/entity/BaseEntity.hpp>
#include <tf2/engine/ClientDll.hpp>
#include <tf2/engine/ModelInfo.hpp>
#include <tf2/client/EntityList.hpp>

#include <tf2/utils/UtlVector.hpp>

#include <tf2/studio/Studio.hpp>
#include <tf2/studio/BoneCache.hpp>
#include <tf2/studio/Model.hpp>


TF2_NAMESPACE_BEGIN();

bool IBaseHandle::valid_entity() const noexcept
{
	return IBaseEntityInternal::GetEntity(*this) != nullptr;
}


IBaseEntityInternal* IBaseEntityInternal::GetLocalPlayer()
{
	static IBaseEntityInternal** local_player =
		Interfaces::SDKManager::Get()->ReadSignature("pLocalPlayer").get<IBaseEntityInternal*>();
	assert(local_player);
	return *local_player;
}

int IBaseEntityInternal::GetHighestEntityIndex()
{
	static int* highest_ent_index = IntPtr(
		Interfaces::SDKManager::Get()->ReadSignature("pCEntityListPtr") + 
		Interfaces::SDKManager::Get()->ReadOffset({ "CEntityList", "offsets" }, "HighestEntityIndex").value_or(0)
		).get<int>();
	assert(highest_ent_index);
	return *highest_ent_index;
}

IBaseEntityInternal* IBaseEntityInternal::GetEntity(IBaseHandle hndl)
{
	assert(Interfaces::ClientList);
	return Interfaces::ClientList->GetClientEntityFromHandle(hndl);
}

IBaseEntityInternal* IBaseEntityInternal::GetEntity(int idx)
{
	assert(Interfaces::ClientList);
	return Interfaces::ClientList->GetClientEntity(idx);
}


bool IBaseEntityInternal::BadLocal()
{
	assert(Interfaces::EngineClient);
	return !Interfaces::EngineClient->IsInGame() || BadEntity(GetLocalPlayer());
}

bool IBaseEntityInternal::IsBaseCombatWeapon() const noexcept
{
	static Utils::IMemberVFuncThunk<bool> is_combat_weapon{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "vtable" }, "IsBaseCombatWeapon").value_or(-1)};
	assert(is_combat_weapon);
	return is_combat_weapon(this);
}

EntityDataMap* IBaseEntityInternal::GetDataMap(bool is_pred) const noexcept
{
	if (is_pred)
	{
		static Utils::IMemberVFuncThunk<EntityDataMap*> get_preddescmap{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "vtable"  }, "GetPredDescMap").value_or(-1) };
		assert(get_preddescmap);
		return get_preddescmap(this);
	}
	else
	{
		static Utils::IMemberVFuncThunk<EntityDataMap*> get_datadescmap{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "vtable"  }, "GetDataDescMap").value_or(-1) };
		assert(get_datadescmap);
		return get_datadescmap(this);
	}
}

const IBoneCache* IBaseEntityInternal::GetBoneCache() const
{
	static Utils::IMemberFuncThunk<IBoneCache*, studiohdr_t*> get_bonecache(Interfaces::SDKManager::Get()->ReadSignature({ "CBaseEntity" }, "GetBoneCache").get());
	assert(get_bonecache);
	return get_bonecache(this, nullptr);
}

bool IBaseEntityInternal::QueryBoneInfo(IBoneInfo& results) const
{
	assert(Interfaces::ModelInfo);

	const ModelInfo* mdl = GetModel();
	if (!mdl)
		return false;

	const studiohdr_t* shdr = Interfaces::ModelInfo->GetStudiomodel(mdl);
	if (!shdr)
		return false;

	const mstudiohitboxset_t* shbs = shdr->GetHitboxSet(HitboxSet);
	if (!shbs)
		return false;

	const IBoneCache* cache = GetBoneCache();

	if (!cache)
		return false;

	results = { cache, shbs, shdr };
	return true;
}

bool IBaseEntityInternal::GetBonePosition(int bone_position, BoneResult& results) const
{
	assert(Interfaces::ModelInfo);

	const ModelInfo* mdl = GetModel();
	if (!mdl)
		return false;

	const studiohdr_t* shdr = Interfaces::ModelInfo->GetStudiomodel(mdl);
	if (!shdr)
		return false;

	const mstudiohitboxset_t* shbs = shdr->GetHitboxSet(HitboxSet);
	if (!shbs)
		return false;

	const IBoneCache* cache = GetBoneCache();

	if (!cache)
		return false;

	const mstudiobbox_t* sbb = shbs->GetHitbox(bone_position);
	const Matrix3x4_F* bone = cache->GetCachedBone(sbb->Bone);

	results.Position = bone->get_column(3);
	results.Min = sbb->Min;
	results.Max = sbb->Max;

	return true;
}

void IBaseEntityInternal::SetModel(int modelidx)
{
	static Utils::IMemberFuncThunk<void, int> set_modelidx(Interfaces::SDKManager::Get()->ReadSignature({ "CBaseEntity" }, "SetModelIndex").get());
	assert(set_modelidx);
	set_modelidx(this, modelidx);
}

bool IBaseEntityInternal::IsHealthKit() const noexcept
{
	const ModelInfo* mdl = this->GetModel();
	return mdl ? strstr(mdl->Name, "healthkit") != nullptr : false;
}

bool IBaseEntityInternal::IsGhost() const noexcept
{
	const ModelInfo* mdl = this->GetModel();
	// models/props_halloween/ghost*.mdl
	return mdl ? strstr(mdl->Name, "n/ghost") != nullptr : false;
}

void IBaseEntityInternal::EstimateAbsVelocity(Vector3D_F& vel) const
{
	static Utils::IMemberFuncThunk<void, Vector3D_F&> estimate_velocity(Interfaces::SDKManager::Get()->ReadSignature({ "CBaseEntity" }, "EstimateAbsVelocity").get());
	assert(estimate_velocity);
	estimate_velocity(this, vel);
}

int IBaseEntityInternal::LookupAttachment(const char* name)
{
	static Utils::IMemberVFuncThunk<int> lookup_attachment{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "vtable"  }, "LookupAttachment").value_or(-1) };
	assert(lookup_attachment);
	return lookup_attachment(this);
}

bool IBaseEntityInternal::GetAttachment(int attach_pt, Vector3D_F& origin, Angle_F& angles)
{
	static Utils::IMemberVFuncThunk<bool, int, Vector3D_F&, Angle_F&> get_attachment{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "vtable"  }, "GetAttachment").value_or(-1) };
	assert(get_attachment);
	return get_attachment(this, attach_pt, origin, angles);
}

bool IBaseEntityInternal::ShouldCollide(Const::EntCollisionGroup groups, uint32_t mask)
{
	static Utils::IMemberVFuncThunk<bool, Const::EntCollisionGroup, uint32_t> should_collide{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseEntity", "vtable"  }, "ShouldCollide").value_or(-1) };
	assert(should_collide);
	return should_collide(this, groups, mask);
}

TF2_NAMESPACE_END();
