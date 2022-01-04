#pragma once

#include "ICheatIFace.hpp"
#include "Defines.hpp"

class AutoBackstab : public ICheatIFace
{
private:
	bool OnAskPluginLoad(tf2::interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;

	void OnPluginPauseChange(bool pausing) override;
	void OnPluginUnload() override;

	px::MHookRes OnCreateMove(px::PassArgs* pArgs);

	void OnSaveConfig(std::vector<px::IPlugin::FileConfigs>& cfg) const override;

	bool OnRender();

	px::ISHookToken m_CreateMove;

	px::ConVar<bool> m_Enabled{ "autostab_enable", false, "Enable auto-backstab" };

	px::ConVar<bool> m_CheckUber{ "autostab_check_uber", true, "Don't backstab when the enemy is invunerable" };
	px::ConVar<bool> m_CheckInvisible{ "autostab_check_visible", true, "Don't backstab when the enemy is invisible" };
};