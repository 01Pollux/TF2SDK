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
	bool m_IsOn{ };
};