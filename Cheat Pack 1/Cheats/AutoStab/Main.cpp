
#include <shadowgarden/interfaces/GameData.hpp>

#include <tf2/entity/BasePlayer.hpp>
#include <tf2/entity/BaseWeapon.hpp>

#include <tf2/client/UserCmd.hpp>

#include "AutoStab.hpp"

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

	config.Client.ClientList = true;
	config.Client.ClientDLL = true;

	return true;
}

void AutoBackstab::OnPluginLoad()
{
	SG::ImGuiLoader->AddCallback(SG::ThisPlugin, "Auto Backstab", std::bind(&AutoBackstab::OnRender, this));
	if (!SG::ThisPlugin->IsPluginPaused())
		m_CreateMove.attach(false, SG::HookOrder::Any, std::bind(&AutoBackstab::OnCreateMove, this, std::placeholders::_2));
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
	if (m_CreateMove.instance())
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
	if (!pCurWpn || pCurWpn->GetWeaponSlot() != TF2::Const::WeaponSlot::Melee)
		return{ };

	if (pCurWpn->ReadyToBackstab)
	{
		if (!m_CheckUber || !m_CheckInvisible)
		{
			TF2::ITFPlayer pEnt(pCurWpn->BackstabVictim);
			if (m_CheckUber && pEnt->IsPlayerInvunerable())
				return{ };

			if (m_CheckInvisible && pEnt->IsPlayerInvisible())
				return{ };
		}

		cmd->Buttons |= TF2::Const::UserCmd::Keys::Attack;
	}
	return{ };
}
