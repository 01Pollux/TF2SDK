
#include <Engine/ClientDll.hpp>
#include <Engine/ClientDll.hpp>

#include <Engine/GlobalVars.hpp>
#include <Entity/BasePlayer.hpp>
#include <Entity/BaseWeapon.hpp>
#include <Entity/ResourceEntity.hpp>

#include "ESP.hpp"

void GlobalESP::RenderPlayerESP(const TF2::ITFPlayer pPlayer, ESPInfo::BoxInfo& box_info)
{
	using namespace TF2;
	ILocalPlayer pMe;
	ESPInfo::Player* esp_info;
	switch (pPlayer->TeamNum)
	{
	case Const::TFTeam::Blue:
		esp_info = &m_PlayerESPInfo[ESPInfo::TeamBlue];
		break;
	case Const::TFTeam::Red:
		esp_info = &m_PlayerESPInfo[ESPInfo::TeamRed];
		break;
	default: return;
	}

	auto iter_espoverride = m_ESPOverride.find(pPlayer->GetEntIndex());
	ESPOverride* esp_override = iter_espoverride == m_ESPOverride.end() ? nullptr : &iter_espoverride->second;

	if (!esp_override && !esp_info->Enable)
		return;
	else if (esp_info->IgnoreCloak && pPlayer->IsPlayerInvisible())
		return;

	double distance = pPlayer->VecOrigin->distance_to(pMe->VecOrigin);
	if (esp_info->MaxDistance > 50.f && esp_info->MaxDistance < distance)
		return;
	else if (!GetBoxInfo(pPlayer, box_info))
		return;

	DrawSharedInfo(
		box_info,
		esp_info,
		esp_override,
		[distance, pPlayer](const ESPInfo::Shared* esp_info, ESPInfo::TextInfo& renderer)
		{
			const ESPInfo::Player* player_esp = static_cast<const ESPInfo::Player*>(esp_info);
			char fmt[64];

			if (player_esp->DrawName)
			{
				PlayerInfo info;
				if (Interfaces::EngineClient->GetPlayerInfo(pPlayer->GetEntIndex(), &info))
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
				sprintf_s(fmt, "Class: %s", Const::TFClassNames[pPlayer->Class.get<size_t>() - 1]);
				renderer.AddText(fmt);
			}

			if (player_esp->DrawTeam)
			{
				sprintf_s(fmt, "Team: %s", Const::TFTeamNames::CStrings[pPlayer->TeamNum.get<size_t>()]);
				renderer.AddText(fmt);
			}

			// Draw uber percentage
			if (player_esp->DrawUberPerc && pPlayer->Class == Const::TFClass::Medic)
			{
				const auto& Weapons = pPlayer->MyWeapons;
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
		this->DrawSharedHealth(box_info, esp_info, TFPlayerResourceEntity::Get()->Health[pPlayer->GetEntIndex()], TFPlayerResourceEntity::Get()->MaxHealth[pPlayer->GetEntIndex()], true);
}
