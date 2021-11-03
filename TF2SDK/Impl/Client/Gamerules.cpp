
#include <Interfaces/GameData.hpp>

#include "Client/GameRules.hpp"

#include "Utils/Thunks.hpp"


TF2_NAMESPACE_BEGIN();

bool IGameRules::ShouldCollide(Const::EntCollisionGroup group0, Const::EntCollisionGroup group1)
{
	static Utils::IMemberVFuncThunk<bool, Const::EntCollisionGroup, Const::EntCollisionGroup> should_collide{
		Interfaces::SDKManager::Get()->ReadOffset({ "CGameRules" }, "ShouldCollide").value_or(-1)
	};
	return should_collide(this, group0, group1);
}

TF2_NAMESPACE_END();
