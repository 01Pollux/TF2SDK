
#include <Engine/ClientDll.hpp>
#include <Engine/ClientDll.hpp>

#include <Engine/GlobalVars.hpp>
#include <Entity/BasePlayer.hpp>
#include <Entity/BaseWeapon.hpp>
#include <Entity/ResourceEntity.hpp>

#include "ESP.hpp"

void GlobalESP::RenderPlayerESP()
{
	using namespace TF2;

	ILocalPlayer pMe;
	ESPInfo::BoxInfo box_info;
	TFPlayerResourceEntity* players = TFPlayerResourceEntity::Get();
	
	for (int i = 1; i < Interfaces::GlobalVars->MaxClients; i++)
	{
		if (!players->IsConnected[i] || !players->IsAlive[i])
			continue;

		ITFPlayer player(i);
		if (pMe == player || player->Class == Const::TFClass::Unknown)
			continue;
		
		ESPInfo::Player* esp_info;
		switch (player->TeamNum)
		{
		case Const::TFTeam::Blue:
			esp_info = &m_PlayerESPInfo[ESPInfo::TeamBlue];
			break;
		case Const::TFTeam::Red:
			esp_info = &m_PlayerESPInfo[ESPInfo::TeamRed];
			break;
		default: continue;
		}

		if (!esp_info->Enable)
			continue;
		else if (esp_info->IgnoreCloak && player->IsPlayerInvisible())
			continue;

		double distance = player->VecOrigin->distance_to(pMe->VecOrigin);
		if (esp_info->MaxDistance > 50.f && esp_info->MaxDistance < distance)
			continue;
		else if (!GetBoxInfo(player, box_info))
			continue;

		DrawSharedInfo(
			box_info,
			esp_info,
			[distance, player](ESPInfo::Shared* esp_info, ESPInfo::TextInfo& renderer)
			{
				ESPInfo::Player* player_esp = static_cast<ESPInfo::Player*>(esp_info);
				char fmt[64];

				if (player_esp->DrawName)
				{
					PlayerInfo info;
					if (Interfaces::EngineClient->GetPlayerInfo(player->entindex(), &info))
					{
						sprintf_s(fmt, "Name: %s", info.Name);
						renderer.AddText(fmt);
					}
				}

				if (player_esp->DrawDistance)
				{
					sprintf_s(fmt, "Distance: %.2f", distance);
					renderer.AddText(fmt);
				}

				if (player_esp->DrawClassIcon)
				{
//					sprintf_s(fmt, "Class: ", Const::TFClassNames[player->Class.get<size_t>() - 1]);
//					renderer.AddText(fmt);
				}
				else if (player_esp->DrawClass)
				{
					sprintf_s(fmt, "Class: %s", Const::TFClassNames[player->Class.get<size_t>() - 1]);
					renderer.AddText(fmt);
				}

				if (player_esp->DrawTeam)
				{
					sprintf_s(fmt, "Team: %s", Const::TFTeamNames::CStrings[player->TeamNum.get<size_t>()]);
					renderer.AddText(fmt);
				}

				// Draw uber percentage
				if (player_esp->DrawUberPerc && player->Class == Const::TFClass::Medic)
				{
					const auto& Weapons = player->MyWeapons;
					for (size_t i = 0; Weapons[i] && i < 3; i++)
					{
						const IBaseWeapon pWep(Weapons[i]);
						if (pWep && pWep->IsClassID(Const::EntClassID::CWeaponMedigun))
						{
							const float flcharge = pWep->ChargeLevel * 100.f;

							if (pWep->ItemDefinitionIndex != 998) // The Vaccinator
								sprintf_s(fmt, "Uber: %.0f%%", flcharge);
							else
								sprintf_s(fmt, "Uber: %.0f%% | Chages: %.0f", flcharge, flcharge / 25.f);

							renderer.AddText(fmt);
							break;
						}
					}
				}
			}
		);

		if (esp_info->HealthbarMode.get() != ESPInfo::HealthBarMode::type::Disabled)
			this->DrawSharedHealth(box_info, esp_info, TFPlayerResourceEntity::Get()->Health[i], TFPlayerResourceEntity::Get()->MaxHealth[i], true);
	}
}
