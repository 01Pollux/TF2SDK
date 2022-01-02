#pragma once

#include "ICheatIFace.hpp"
#include "Defines.hpp"

class AutoBackstab : public ICheatIFace
{
private:
	bool OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;

	void OnPluginPauseChange(bool pausing) override;
	void OnPluginUnload() override;

	SG::MHookRes OnCreateMove(SG::PassArgs* pArgs);

	void OnSaveConfig(std::vector<SG::IPlugin::FileConfigs>& cfg) const override;

	bool OnRender();

	SG::ISHookToken m_CreateMove;

	SG::ConVar<bool> m_Enabled{ "autostab_enable", false, "Enable auto-backstab" };

	SG::ConVar<bool> m_CheckUber{ "autostab_check_uber", true, "Don't backstab when the enemy is invunerable" };
	SG::ConVar<bool> m_CheckInvisible{ "autostab_check_visible", true, "Don't backstab when the enemy is invisible" };
};