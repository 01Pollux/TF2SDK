#pragma once

#include "ICheatIFace.hpp"
#include "Defines.hpp"

class InventoryManager : public ICheatIFace
{
private:
	bool OnAskPluginLoad(tf2::Interfaces::SDKManager::Config& config) override;
	void OnPluginLoad() override;

	void OnPluginPauseChange(bool pausing) override;
	void OnPluginUnload() override;

	px::MHookRes OnCreateMove(px::PassArgs* pArgs);

	void OnSaveConfig(std::vector<px::IPlugin::FileConfigs>& cfg) const override;

	bool OnRender();

	px::ISHookToken m_FrameStageNotify;
};