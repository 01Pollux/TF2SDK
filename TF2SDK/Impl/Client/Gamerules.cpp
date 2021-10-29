
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

Const::GameType IGameRules::GetGameType() const
{
	constexpr Const::EntClassID gamerules = Const::EntClassID::CTFGameRulesProxy;
	uint32_t offset{ };
	using enum Const::GameType;
	
	{
		for (auto [name, type] : {
			std::pair{ "m_bIsInTraining", Training },
			std::pair{ "m_bPlayingSpecialDeliveryMode", SpecialModeDelivery },
			std::pair{ "m_bPlayingMannVsMachine", Mvm },
			std::pair{ "m_bPlayingKoth", Koth },
			std::pair{ "m_bPlayingRobotDestructionMode", RobotDestruction },
		})
		{
			if (PropFinder::FindRecvProp(gamerules, name, nullptr, &offset))
			{
				if (*std::bit_cast<bool*>(std::bit_cast<uintptr_t>(this) + offset))
					return type;
			}
		}

		switch (PropFinder::FindRecvProp(gamerules, "m_nGameType", nullptr, &offset))
		{
		case 1:	return CaptureTheFlag;
		default: break;
		}

		return Deathmatch;
	}

}


TF2_NAMESPACE_END();
