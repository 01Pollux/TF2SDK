
#include "Entity/ResourceEntity.hpp"

TF2_NAMESPACE_BEGIN();

TFPlayerResourceEntity* TFPlayerResourceEntity::Get()
{
	static TFPlayerResourceEntity* resource_ptr = 
		Interfaces::SDKManager::Get()->ReadSignature({ "CBaseEntity" }, "PlayerResource").get<TFPlayerResourceEntity>();
	assert(resource_ptr);
	return resource_ptr;
}


TFMonsterResourceEntity* TFMonsterResourceEntity::Get()
{
	static TFMonsterResourceEntity* resource_ptr =
		Interfaces::SDKManager::Get()->ReadSignature({ "CBaseEntity" }, "MonsterResource").get<TFMonsterResourceEntity>();
	assert(resource_ptr);
	return resource_ptr;
}

TF2_NAMESPACE_END();
