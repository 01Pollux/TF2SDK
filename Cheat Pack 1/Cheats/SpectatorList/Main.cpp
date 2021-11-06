
#include <Interfaces/GameData.hpp>

#include "DisplayDead.hpp"

#include <Entity/BasePlayer.hpp>
#include <Entity/BaseWeapon.hpp>

#include <Client/UserCmd.hpp>

static DisplaySpecList auto_stab;

bool DisplaySpecList::OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config)
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

void DisplaySpecList::OnPluginLoad()
{
	SG::ImGuiLoader->AddCallback(SG::ThisPlugin, "Spectator list", std::bind(&DisplaySpecList::OnRender, this));
}


void DisplaySpecList::OnPluginPauseChange(bool pausing)
{
	if (!pausing)
		m_CreateMove.attach(false, SG::HookOrder::Any, std::bind(&DisplaySpecList::OnCreateMove, this, std::placeholders::_2));
	else
		m_CreateMove.detach();
}


void DisplaySpecList::OnPluginUnload()
{
	if (m_CreateMove)
		m_CreateMove.detach();
	SG::DetourManager->ReleaseHook(m_CreateMove.instance());
}

SG::MHookRes DisplaySpecList::OnCreateMove(SG::PassArgs* pArgs)
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
