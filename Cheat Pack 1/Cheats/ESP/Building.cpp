
#include <tf2/engine/GlobalVars.hpp>
#include <tf2/engine/ClientDll.hpp>

#include <tf2/entity/BasePlayer.hpp>

#include "ESP.hpp"

void GlobalESP::RenderBuildingESP(const tf2::IBaseObject pEnt, tf2::Const::EntClassID class_id, ESPInfo::BoxInfo& box_info, int ent_index)
{
	using namespace tf2;

	ILocalPlayer pMe;
	ESPInfo::Building* esp_info;

	switch (pEnt->TeamNum)
	{
	case Const::TFTeam::Blue:
		esp_info = &m_BuildingESPInfo[ESPInfo::TeamBlue];
		break;
	case Const::TFTeam::Red:
		esp_info = &m_BuildingESPInfo[ESPInfo::TeamRed];
		break;
	default: return;
	}

	if (!esp_info->Enable)
		return;

	double distance = pEnt->VecOrigin->distance_to(pMe->VecOrigin);
	if (esp_info->MaxDistance > 50.f && esp_info->MaxDistance < distance)
		return;
	else if (!GetBoxInfo(pEnt, box_info))
		return;

	auto iter_espoverride = m_ESPOverride.find(ent_index);
	ESPOverride* esp_override = iter_espoverride == m_ESPOverride.end() ? nullptr : &iter_espoverride->second;

	DrawSharedInfo(
		box_info,
		esp_info,
		esp_override,
		[distance, pEnt, class_id](const ESPInfo::Shared* esp_info, ESPInfo::TextInfo& renderer)
		{
			const ESPInfo::Building* building_esp = static_cast<const ESPInfo::Building*>(esp_info);
			char fmt[64];

			switch (class_id)
			{
			case Const::EntClassID::CObjectSentrygun:
			{
				ISentryGun sentry{ pEnt };

				if (building_esp->DrawName)
					renderer.AddText("Sentrygun");

				if (building_esp->DrawBState && pEnt->HasSapper)
					renderer.AddText("<Sapped>");

				if (building_esp->DrawLevel)
				{
					if (sentry->IsMiniBuilding)
						renderer.AddText("<Mini>");
					else
					{
						sprintf_s(fmt, "Level: %i", sentry->UpgradeLevel.get());
						renderer.AddText(fmt);
					}
				}

				if (building_esp->DrawBState)
				{
					if (sentry->IsPlayerControlled)
						renderer.AddText("<Wrangled>");
				}

				if (building_esp->DrawAmmo)
				{
					sprintf_s(fmt, "Ammo: %i / %i", sentry->AmmoShells.get(), sentry->MaxAmmoShells.get());
					renderer.AddText(fmt);

					if (sentry->UpgradeLevel == 3)
					{
						sprintf_s(fmt, "Rockets: %i / 20", sentry->AmmoRockets.get());
						renderer.AddText(fmt);
					}
				}

				break;
			}
			
			case Const::EntClassID::CObjectDispenser:
			{
				IDispenser dispenser{ pEnt };

				if (building_esp->DrawName)
					renderer.AddText("Dispenser");

				if (building_esp->DrawBState && pEnt->HasSapper)
					renderer.AddText("<Sapped>");

				if (building_esp->DrawLevel)
				{
					sprintf_s(fmt, "Level: %i", dispenser->UpgradeLevel.get());
					renderer.AddText(fmt);
				}

				if (building_esp->DrawBState)
				{
					if (const uint32_t healing_players = dispenser->HealingTargets->size())
					{
						sprintf_s(fmt, "Healing: %u Players", healing_players);
						renderer.AddText(fmt);
					}
				}

				if (building_esp->DrawAmmo)
				{
					sprintf_s(fmt, "Ammo: %i", dispenser->AmmoMetal.get());
					renderer.AddText(fmt);
				}

				break;
			}

			case Const::EntClassID::CObjectTeleporter:
			{
				ITeleporter teleporter{ pEnt };

				if (building_esp->DrawName)
					renderer.AddText("Teleporter");

				if (building_esp->DrawBState && pEnt->HasSapper)
					renderer.AddText("<Sapped>");

				if (building_esp->DrawLevel)
				{
					sprintf_s(fmt, "Level: %i", teleporter->UpgradeLevel.get());
					renderer.AddText(fmt);
				}

				if (building_esp->DrawBState)
				{
					if (const int time_used = teleporter->TimesUsed)
					{
						sprintf_s(fmt, "Used: %i", time_used);
						renderer.AddText(fmt);
					}

					switch (teleporter->State)
					{
					case Const::TeleporterState::Idle:
					{
						renderer.AddText("<Ready>");
						break;
					}

					case Const::TeleporterState::Recharging:
					{
						if (teleporter->RechargeTime)
						{
							float recharge_elapsed = teleporter->RechargeTime - interfaces::GlobalVars->CurTime;
							sprintf_s(fmt, "Charging: %.1f", recharge_elapsed);
							renderer.AddText(fmt);
						}
						break;
					}

					}
				}

				break;
			}
			}

			if (building_esp->DrawOwner)
			{
				if (ITFPlayer player{ pEnt->Builder })
				{
					PlayerInfo info;
					if (interfaces::EngineClient->GetPlayerInfo(player->GetEntIndex(), &info))
					{
						sprintf_s(fmt, "Owner: %s", info.Name);
						renderer.AddText(fmt);
					}
				}
			}

			if (building_esp->DrawBState)
			{
				if (pEnt->IsCarried)
					renderer.AddText("<Carried>");
				else if (pEnt->IsBuilding)
					renderer.AddText("<Building>");
			}
		}
	);

	if (esp_info->HealthbarMode.get() != ESPInfo::HealthBarMode::type::Disabled)
		this->DrawSharedHealth(box_info, esp_info, pEnt->Health, pEnt->MaxHealth, false);
}
