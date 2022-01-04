#include <px/interfaces/InterfacesSys.hpp>
#include <imgui/imgui_internal.h>

#include "Defines.hpp"
#include "ICheatIFace.hpp"

class IGlobalCheatIFace : public px::IPluginImpl
{
public:
	void OnPluginPreLoad(px::IPluginManager* ifacemgr) override
	{
		for (auto& iface : ICheatIFace::GetEntries())
			iface->OnPluginPreLoad(ifacemgr);
	}

	bool OnPluginLoad2(px::IPluginManager* ifacemgr) override
	{ 
		ImGui::SetCurrentContext(px::ImGuiLoader->GetContext());

		tf2::interfaces::SDKManager::Config tf2_config;
		for (auto iface : ICheatIFace::GetEntries())
		{
			if (!iface->OnAskPluginLoad(tf2_config))
				return false;
		}

		if (!m_TF2Sdk.init(px::LibManager->OpenGameData(this), tf2_config))
			return false;

		for (auto iface : ICheatIFace::GetEntries())
			iface->OnPluginLoad();

		return true;
	}

	void OnPluginUnload() override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnPluginUnload();
	}

	void OnAllPluginsLoaded() override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnAllPluginsLoaded();
	}

	void OnPluginPauseChange(bool pausing) override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnPluginPauseChange(pausing);
	}

	void OnSaveConfig(std::vector<px::IPlugin::FileConfigs>& cfg) const override
	{
		for (auto iface : ICheatIFace::GetEntries())
			iface->OnSaveConfig(cfg);
	}

private:
	tf2::interfaces::SDKManager m_TF2Sdk;
};

// Important for exporting plugin
PX_DLL_EXPORT(IGlobalCheatIFace);
