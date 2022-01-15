
#include <px/interfaces/GameData.hpp>

#include <tf2/entity/BasePlayer.hpp>
#include <tf2/entity/BaseWeapon.hpp>
#include <tf2/engine/GameTrace.hpp>
#include <tf2/engine/ClientDll.hpp>

#include <tf2/client/UserCmd.hpp>

#include "AutoStab.hpp"

static AutoBackstab auto_stab;

bool AutoBackstab::OnAskPluginLoad(tf2::interfaces::SDKManager::Config& config)
{
	std::unique_ptr<px::IGameData> game_data{ px::LibManager->OpenGameData(px::ThisPlugin) };
	
	px::IntPtr clientmode = game_data->ReadSignature("ClientModePointer");
	m_CreateMove = px::DetourManager->LoadHook({ "CHLClient" }, "CreateMove", clientmode.get(), game_data.get());

	if (!m_CreateMove.instance())
	{
		PX_LOG_FATAL(PX_MESSAGE("Failed to Load hook for: \"CHLClient::CreateMove\""));
		return false;
	}

	config.Client.ClientList = true;
	config.Client.ClientDLL = true;

	return true;
}

void AutoBackstab::OnPluginLoad()
{
	px::ImGuiLoader->AddCallback(px::ThisPlugin, "Auto Backstab", std::bind(&AutoBackstab::OnRender, this));
	if (!px::ThisPlugin->IsPluginPaused())
		m_CreateMove.attach(false, px::HookOrder::Any, std::bind(&AutoBackstab::OnCreateMove, this, std::placeholders::_2));
}


void AutoBackstab::OnPluginPauseChange(bool pausing)
{
	if (!pausing)
		m_CreateMove.attach(false, px::HookOrder::Any, std::bind(&AutoBackstab::OnCreateMove, this, std::placeholders::_2));
	else
		m_CreateMove.detach();
}


void AutoBackstab::OnPluginUnload()
{
	if (m_CreateMove)
		m_CreateMove.detach();
	if (m_CreateMove.instance())
		px::DetourManager->ReleaseHook(m_CreateMove.instance());
}

px::MHookRes AutoBackstab::OnCreateMove(px::PassArgs* pArgs)
{
	if (!m_Enabled)
		return{ };

	auto cmd = pArgs->get<tf2::Const::UserCmd*>(1);
	if (!cmd || !cmd->CmdNumber)
		return{ };

	tf2::ILocalPlayer pMe;
	if (pMe->Class != tf2::Const::TFClass::Spy)
		return{ };

	if (pMe->FeignDeathReady)
		return{ };

	tf2::IBaseWeapon pCurWpn(pMe->ActiveWeapon.get());

	static tf2::Const::WeaponSlot last_slot = tf2::Const::WeaponSlot::Misc;
	if (pCurWpn && pCurWpn->GetWeaponSlot() != last_slot)
	{
		last_slot = pCurWpn->GetWeaponSlot();
		tf2::interfaces::EngineClient->Con_NPrintf(0, "Weapon: %x / %i / slot = %i", pCurWpn.get(), static_cast<int>(pCurWpn->ItemDefinitionIndex), last_slot);
	}

	if (!pCurWpn || pCurWpn->GetWeaponSlot() != tf2::Const::WeaponSlot::Melee)
		return{ };

	if (pCurWpn->ReadyToBackstab)
	{
		if (m_CheckUber || m_CheckInvisible)
		{
			tf2::GameTrace trace;
			if (pCurWpn->DoSwingTrace(trace) && trace.Entity)
			{
				tf2::ITFPlayer pTarget{ trace.Entity };
				if (m_CheckUber && pTarget->IsPlayerInvunerable())
					return{ };

				if (m_CheckInvisible && pTarget->IsPlayerInvisible())
					return{ };
			}
			else return{ };
		}

		cmd->Buttons |= tf2::Const::UserCmd::Keys::Attack;
	}

	return{ };
}