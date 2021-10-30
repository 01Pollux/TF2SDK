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

	void OnSaveConfig(Json& cfg);

	void OnReloadConfig(const Json& cfg);

	bool OnRender();

	SG::ISHookToken m_CreateMove;

	SG::Config<bool> m_Enabled{ "Enable", false, "Enable backtrack" };
	SG::Config<bool> m_CheckUber{ "Check Ubercharge", true, "Don't backstab when the enemy is invunerable" };
	SG::Config<bool> m_CheckInvisible{ "Check visible", true, "Don't backstab when the enemy is invisible" };
	bool m_IsOn{ };
};