
#include <array>
#include <Interfaces/GameData.hpp>

#include "AutoStab.hpp"

#include <Entity/BasePlayer.hpp>
#include <Entity/BaseWeapon.hpp>

#include <Client/UserCmd.hpp>

#include <VGui/Engine.hpp>
#include <VGui/Panel.hpp>

static AutoBackstab auto_stab;

bool AutoBackstab::OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config)
{
	std::unique_ptr<SG::IGameData> game_data{ SG::LibManager->OpenGameData(SG::ThisPlugin) };
	
	SG::IntPtr clientmode = game_data->ReadSignature("ClientModePointer");
	m_CreateMove = SG::DetourManager->LoadHook({ "CHLClient" }, "CreateMove", clientmode.get(), game_data.get());

	if (!m_CreateMove.instance())
	{
		SG_LOG_FATAL(SG_MESSAGE("Failed to Load hook for: \"CHLClient::CreateMove\""));
		return false;
	}

	config.Engine.Convar = true;

	config.Client.ClientList = true;
	config.Client.ClientDLL = true;

	config.Server.ServerTools = true;

	config.VGui.Panel = true;

	return true;
}

void AutoBackstab::OnPluginLoad()
{
	SG::ImGuiLoader->AddCallback(SG::ThisPlugin, "Auto Backstab", std::bind(&AutoBackstab::OnRender, this));
}


void AutoBackstab::OnPluginPauseChange(bool pausing)
{
	if (!pausing)
		m_CreateMove.attach(false, SG::HookOrder::Any, std::bind(&AutoBackstab::OnCreateMove, this, std::placeholders::_2));
	else
		m_CreateMove.detach();
}


void AutoBackstab::OnPluginUnload()
{
	if (m_CreateMove)
		m_CreateMove.detach();
	SG::DetourManager->ReleaseHook(m_CreateMove.instance());
}


SG::MHookRes AutoBackstab::OnCreateMove(SG::PassArgs* pArgs)
{
	auto cmd = pArgs->get<TF2::Const::UserCmd*>(1);
	if (!cmd || !cmd->CmdNumber)
		return{ };

	TF2::ILocalPlayer pMe;
	if (pMe->Class != TF2::Const::TFClass::Spy)
		return{ };

	if (pMe->FeignDeathReady)
		return{ };

	TF2::IBaseWeapon pCurWpn(pMe->ActiveWeapon.get());
	if (!pCurWpn || pCurWpn->GetWeaponSlot() != 2)
		return{ };

	if (pCurWpn->ReadyToBackstab)
	{
		if (!m_CheckUber || !m_CheckInvisible)
		{
			TF2::ITFPlayer pEnt(pCurWpn->BackstabVictim);
			if (m_CheckUber)
			{
				constexpr std::array invun_conds{
					TF2::Const::ETFCond::Ubercharged,
					TF2::Const::ETFCond::UberchargedCanteen,
					TF2::Const::ETFCond::UberchargedOnTakeDamage,
					TF2::Const::ETFCond::Bonked,
					TF2::Const::ETFCond::DefenseBuffMmmph,
				};

				for (auto cond : invun_conds)
				{
					if (pEnt->InCond(cond))
						return{ };
				}
			}

			if (m_CheckInvisible)
			{
				constexpr std::array invun_conds{
					TF2::Const::ETFCond::Cloaked,
					TF2::Const::ETFCond::Stealthed
				};

				for (auto cond : invun_conds)
				{
					if (pEnt->InCond(cond))
						return{ };
				}
			}
		}
		
		cmd->Buttons |= TF2::Const::UserCmd::Keys::Attack;
	}
	return{ };
}
