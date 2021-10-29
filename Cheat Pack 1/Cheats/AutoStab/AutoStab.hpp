#pragma once

#include "ICheatIFace.hpp"

class AutoBackstab : public ICheatIFace
{
public:
	bool OnAskPluginLoad(TF2::Interfaces::SDKManager::Config& config) override;

	void OnPluginLoad() override;

private:
	void OnRender();
	bool m_IsOn{ };
};